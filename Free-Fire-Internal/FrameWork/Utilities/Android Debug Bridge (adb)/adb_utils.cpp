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
            // Búfer para formatear los mensajes
            char msgBuffer[1024];

            //MessageBoxA(NULL, "Iniciando el proceso de conexion con ADB...", "Debug ADB: Paso 1", MB_OK | MB_ICONINFORMATION);

            // --- Búsqueda de la ruta de ADB ---
            std::string adbPath = GetAdbPath();
            if (adbPath.empty()) {
                MessageBoxA(NULL, "ERROR: La ruta del ejecutable 'adb.exe' no fue encontrada.", "Debug ADB: ERROR en Paso 2", MB_OK | MB_ICONERROR);
                return false;
            }

            sprintf_s(msgBuffer, sizeof(msgBuffer), "ACIERTO: Se encontro la ruta de ADB.\n\nRuta: %s", adbPath.c_str());
            //MessageBoxA(NULL, msgBuffer, "Debug ADB: Paso 2", MB_OK | MB_ICONINFORMATION);

            // --- Inicialización y obtención de la dirección de la librería ---
            FrameWork::Adb adb(adbPath);
            //MessageBoxA(NULL, "Iniciando ADB y buscando la libreria 'libil2cpp.so' en 'com.dts.freefireth'...", "Debug ADB: Paso 3", MB_OK | MB_ICONINFORMATION);

            Context::LibraryAddress = adb.Start(XorStr("com.dts.freefireth"), XorStr("libil2cpp.so"));

            adb.Kill(); // Se cierra el proceso de ADB

            // --- Verificación de la dirección de la librería ---
            if (Context::LibraryAddress == 0) {
                MessageBoxA(NULL, "ERROR: No se pudo obtener la direccion de la libreria 'libil2cpp.so'.\n\nPosibles causas:\n1. El juego no esta abierto o no es la version correcta.\n2. El emulador no tiene la depuracion USB activada.\n3. El paquete no es 'com.dts.freefireth'.", "Debug ADB: ERROR en Paso 4", MB_OK | MB_ICONERROR);
                // El throw original detendría la ejecución, lo cambiamos por un return para una salida más limpia en depuración.
                return false;
            }

            sprintf_s(msgBuffer, sizeof(msgBuffer), "ACIERTO: Se obtuvo la direccion de la libreria.\n\nDireccion de libil2cpp.so: 0x%p", (void*)Context::LibraryAddress);
            //MessageBoxA(NULL, msgBuffer, "Debug ADB: Paso 4", MB_OK | MB_ICONINFORMATION);

            // --- Lectura de verificación de la librería ---
            //MessageBoxA(NULL, "Intentando leer el primer byte de la libreria para verificar...", "Debug ADB: Paso 5", MB_OK | MB_ICONINFORMATION);

            byte elf = FrameWork::Memory::Read<byte>(Context::LibraryAddress);

            std::stringstream ss;
            ss << "ACIERTO FINAL: Lectura de memoria exitosa.\n\n";
            ss << "Byte leido (ELF): " << static_cast<unsigned int>(elf) << "\n";
            ss << "Byte (Hex): 0x" << std::hex << static_cast<unsigned int>(elf) << "\n\n";
            ss << "La inicializacion por ADB fue completamente exitosa.";

            //MessageBoxA(NULL, ss.str().c_str(), "Debug ADB: Paso 5", MB_OK | MB_ICONINFORMATION);

            return true;
        }
    }
}