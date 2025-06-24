#include <Windows.h>
#include <thread>
#include <Cheat/Cheat.hpp>
#include <FrameWork/FrameWork.hpp>
#include <MinHook.h> // Añade el include de MinHook aquí

HMODULE g_hModule;
bool bShouldUnload = false;
FrameWork::Interface* g_pInterface = nullptr;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {

#ifdef _DEBUG
    FrameWork::Misc::InitializeConsole();
#endif // DEBUG

    Cheat::Initialize();

    while (!bShouldUnload || !g_Options.General.ShutDown) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    FrameWork::Misc::ShutDownConsole();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        // Aquí es donde probablemente inicias tu cheat en un hilo
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)wWinMain, hModule, 0, nullptr);
        break;

    case DLL_PROCESS_DETACH:
        // --- AÑADE ESTO ---
        // Esto es crucial. Deshabilita todos los hooks y libera los recursos de MinHook.
        MH_Uninitialize();
        break;
        
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}