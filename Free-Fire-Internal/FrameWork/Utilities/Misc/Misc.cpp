#include <FrameWork/Utilities/Misc/Misc.hpp>
#include <FrameWork/FrameWork.hpp>
#include <algorithm>

namespace FrameWork {
    namespace Misc {
        int RandomInt(int Lower, int Max) {
            return (rand() % (Max - Lower + 1)) + Lower;
        }

        std::wstring RandomString(size_t Length) {
            auto Randchar = []() -> char {
                const char* Charset = XorStr("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
                const size_t MaxIndex = (sizeof(Charset) - 1);
                return Charset[rand() % MaxIndex];
                };

            std::wstring Str(Length, 0);
            std::generate_n(Str.begin(), Length, Randchar);
            return Str;
        }

        std::string Wstring2String(std::wstring Input) {
            return std::string(Input.begin(), Input.end());
        }

        std::string convert_u8string(std::u8string u8str) {
            return std::string(u8str.begin(), u8str.end());
        }

        std::wstring String2WString(std::string Input) {
            return std::wstring(Input.begin(), Input.end());
        }

        void InitializeConsole() {
            SafeCall(AllocConsole)();
            SetConsoleOutputCP(CP_UTF8);
            SetConsoleCP(CP_UTF8);
            freopen(XorStr("CONIN$"), "r", stdin);
            freopen(XorStr("CONOUT$"), "w", stdout);
            freopen(XorStr("CONOUT$"), "w", stderr);
            SafeCall(SetConsoleTitleA)(XorStr("Debug Console"));
        }

        void ShutDownConsole() {
            SafeCall(FreeConsole)();
            SafeCall(fclose)(stdout);
        }
    }
}