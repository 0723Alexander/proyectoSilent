#include <FrameWork/Utilities/FindRender/Render.hpp>
#include <string>

HWND RenderWindow;

namespace FrameWork {
        std::vector<DWORD> GetProcessIdsByName(const std::wstring& ProcessName)
        {
            std::vector<DWORD> processIds;

            HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (hSnapshot == INVALID_HANDLE_VALUE) {
                return processIds;
            }

            PROCESSENTRY32 processEntry;
            processEntry.dwSize = sizeof(PROCESSENTRY32);

            if (Process32First(hSnapshot, &processEntry))
            {
                do {
                    if (ProcessName == processEntry.szExeFile)
                    {
                        processIds.push_back(processEntry.th32ProcessID);
                    }
                } while (Process32Next(hSnapshot, &processEntry));
            }

            CloseHandle(hSnapshot);

            return processIds;
        }

        inline BOOL CALLBACK EnumChildWindowsProc(HWND hWnd, LPARAM lParam)
        {
            char windowName[256];
            GetWindowTextA(hWnd, windowName, sizeof(windowName) / sizeof(windowName[0]));
            std::string name(windowName);

            char className[256];
            GetClassNameA(hWnd, className, sizeof(className) / sizeof(className[0]));
            std::string class_str(className);

            if (name == "BlueStacks Android PluginAndroid" && class_str.find("WindowsForms10") != std::wstring::npos)
            {
                RenderWindow = hWnd;
                return FALSE;
            }
            else if (name == "HD-Player" && class_str.find("Qt") != std::wstring::npos)
            {
                RenderWindow = hWnd;
                return FALSE;
            }

            return TRUE;
        }

        inline BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
        {
            DWORD processId = *reinterpret_cast<DWORD*>(lParam);

            DWORD windowProcessId;
            GetWindowThreadProcessId(hWnd, &windowProcessId);

            if (windowProcessId == processId)
            {
                EnumChildWindows(hWnd, EnumChildWindowsProc, lParam);
            }

            return TRUE;
        }

        HWND FindeRender::FindRenderWindow()
        {
            auto BlueStacksProcesses = GetProcessIdsByName(L"Bluestacks.exe");
            if (BlueStacksProcesses.empty()) {
                BlueStacksProcesses = GetProcessIdsByName(L"HD-Player.exe");
            }
            for (const auto& ProcessId : BlueStacksProcesses)
            {
                HANDLE ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId);
                if (ProcessHandle)
                {
                    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&ProcessId));

                    CloseHandle(ProcessHandle);
                }
            }

            return RenderWindow;
        }
};