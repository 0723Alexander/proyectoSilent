#include <FrameWork/Utilities/Android Debug Bridge (adb)/adb_utils.hpp>

namespace FrameWork {
    namespace ADB {

        std::string GetProcessPath(const std::wstring& processName) {
            PROCESSENTRY32 entry;
            entry.dwSize = sizeof(PROCESSENTRY32);
            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

            if (snapshot == INVALID_HANDLE_VALUE) return "";

            std::string processPath = "";
            if (Process32First(snapshot, &entry)) {
                do {
                    if (std::wstring(entry.szExeFile) == processName) {
                        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, entry.th32ProcessID);
                        if (hProcess) {
                            char buffer[MAX_PATH];
                            if (GetModuleFileNameExA(hProcess, NULL, buffer, MAX_PATH)) {
                                processPath = buffer;
                            }
                            CloseHandle(hProcess);
                        }
                        break;
                    }
                } while (Process32Next(snapshot, &entry));
            }

            CloseHandle(snapshot);
            return processPath;
        }

        std::vector<std::string> GetAvailableDrives() {
            std::vector<std::string> drives;
            DWORD driveMask = GetLogicalDrives();

            for (char letter = 'A'; letter <= 'Z'; ++letter) {
                if (driveMask & (1 << (letter - 'A'))) {
                    std::string drive = { letter, ':', '\\' };
                    drives.push_back(drive);
                }
            }
            return drives;
        }

        std::string FindAdbPath(const std::string& emulatorFolder) {
            std::vector<std::string> drives = GetAvailableDrives();

            for (const auto& drive : drives) {
                std::string adbPath = drive + XorStr("Program Files\\") + emulatorFolder + XorStr("\\HD-Adb.exe");
                if (std::filesystem::exists(adbPath)) {
                    return adbPath;
                }
            }
            return "";
        }

        std::string GetAdbPath() {
            std::string hdPlayerPath = GetProcessPath(XorStr(L"HD-Player.exe"));
            std::string blueStacksPath = GetProcessPath(XorStr(L"Bluestacks.exe"));

            if (!hdPlayerPath.empty()) {
                if (hdPlayerPath.find(XorStr("BlueStacks_msi5")) != std::string::npos) {
                    return FindAdbPath(XorStr("BlueStacks_msi5"));
                }
                else if (hdPlayerPath.find(XorStr("BlueStacks_msi2")) != std::string::npos) {
                    return FindAdbPath(XorStr("BlueStacks_msi2"));
                }
                else if (hdPlayerPath.find(XorStr("BlueStacks_nxt")) != std::string::npos) {
                    return FindAdbPath(XorStr("BlueStacks_nxt"));
                }
                else if (hdPlayerPath.find(XorStr("BlueStacks")) != std::string::npos) {
                    return FindAdbPath(XorStr("BlueStacks"));
                }
            }

            if (!blueStacksPath.empty() && blueStacksPath.find(XorStr("BlueStacks")) != std::string::npos) {
                return FindAdbPath(XorStr("BlueStacks"));
            }
            std::string adbPathMSI5 = FindAdbPath(XorStr("BlueStacks_msi5"));
            std::string adbPathMSI2 = FindAdbPath(XorStr("BlueStacks_msi2"));
            std::string adbPathNXT = FindAdbPath(XorStr("BlueStacks_nxt"));
            std::string adbPathBlueStacks = FindAdbPath(XorStr("BlueStacks"));

            if (!adbPathMSI5.empty()) {
                return adbPathMSI5;
            }
            if (!adbPathMSI2.empty()) {
                return adbPathMSI2;
            }
            if (!adbPathNXT.empty()) {
                return adbPathNXT;
            }
            return adbPathBlueStacks;
        }

        bool InitializeADB() {
            FrameWork::Misc::Log(XorStr("[FreeFire-Cheat] Connecting ADB..."));

            std::string adbPath = GetAdbPath();
            if (adbPath.empty()) {
                FrameWork::Misc::Log(XorStr("Error: ADB path not found."));
                return false;
            }

            FrameWork::Adb adb(adbPath);
            FrameWork::Misc::Log(XorStr("[FreeFire-Cheat] ADB Path Detected: ") + adbPath);
            Context::LibraryAddress = adb.Start(XorStr("com.dts.freefireth"), XorStr("libil2cpp.so"));

            adb.Kill();

            FrameWork::Misc::Log(XorStr("[FreeFire-Cheat] Library Address: ") + std::to_string(Context::LibraryAddress));
            if (Context::LibraryAddress == 0) {
                throw std::runtime_error(XorStr("Error: Failed to find library address."));
            }

            FrameWork::Misc::Log(XorStr("[FreeFire-Cheat] Reading Library"));
            byte elf = FrameWork::Memory::Read<byte>(Context::LibraryAddress);
            std::stringstream ss;
            ss << XorStr("ELF (Decimal): ") << static_cast<unsigned int>(elf)
                << XorStr(" | ELF (Hexadecimal): 0x") << std::hex << static_cast<unsigned int>(elf);
            FrameWork::Misc::Log(ss.str());

            return true;
        }
    }
}