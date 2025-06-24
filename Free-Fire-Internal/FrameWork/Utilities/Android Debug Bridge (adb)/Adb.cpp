#include <FrameWork/Utilities/Android Debug Bridge (adb)/Adb.hpp>

using namespace Poco;

namespace FrameWork {
    Adb::Adb(const std::string& path) : _path(path) {}

    void Adb::Kill() {
        std::vector<std::wstring> names = { XorStr(L"adb.exe"), XorStr(L"HD-Adb.exe") };

        for (const auto& name : names) {
            KillProcessesByName(name);
        }
    }

    uint32_t Adb::Start(std::string processName, std::string moduleName) {
        ExecuteAdbCommand(XorStr("kill-server"));
        ExecuteAdbCommand(XorStr("devices"));

        Pipe inPipe, outPipe, errPipe;
        Process::Args args;
        args.push_back(XorStr("shell"));
        args.push_back(XorStr("getprop ro.secure ; /boot/android/android/system/xbin/bstk/su"));

        Process::launch(
            _path,
            args,
            &inPipe,
            &outPipe,
            &errPipe
        );

        PipeInputStream pis(outPipe);
        PipeInputStream pisErr(errPipe);
        PipeOutputStream pos(inPipe);
        std::string line;

        bool foundPid = false;

        while (std::getline(pis, line)) {
            if (line.find(XorStr("0")) == 0) {
                pos << XorStr("ps") << std::endl;
                pos.flush();
            }

            if (line.find(processName) != std::string::npos && !foundPid) {
                auto parts = split(line, ' ');
                if (parts.size() > 1) {
                    auto pid = parts[4];

                    std::string mapInput = XorStr("cat /proc/$(pidof com.dts.freefireth)/maps | grep ") + moduleName;

                    foundPid = true;
                    pos << mapInput << std::endl;
                    pos.flush();
                }
            }

            if (line.find(moduleName) != std::string::npos) {
                auto moduleParts = split(line, '-');
                auto il2cpp = moduleParts[0];

                return std::stoul(il2cpp, nullptr, 16);
            }
        }

        return 0;
    }

    void Adb::ExecuteAdbCommand(const std::string& command) {
        Process::Args args;
        args.push_back(command);
        Pipe inPipe, outPipe, errPipe;

        ProcessHandle proc = Process::launch(
            _path,
            args,
            &inPipe,
            &outPipe,
            &errPipe
        );

        PipeInputStream pis(outPipe);
        PipeInputStream pisErr(errPipe);
        PipeOutputStream pos(inPipe);

        proc.wait();
    }

    void Adb::KillProcessesByName(const std::wstring& processName) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);

        if (!Process32First(hSnapshot, &pe)) {
            CloseHandle(hSnapshot);
        }

        do {
            if (processName == pe.szExeFile) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                if (hProcess) {
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                }
            }
        } while (Process32Next(hSnapshot, &pe));

        CloseHandle(hSnapshot);
    }

    std::vector<std::string> Adb::split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
}