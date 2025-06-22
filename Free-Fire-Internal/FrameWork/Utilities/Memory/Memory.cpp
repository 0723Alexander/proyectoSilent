#include <FrameWork/Utilities/Memory/Memory.hpp>
#include <include/MinHook.h>

namespace FrameWork {

    int __cdecl Memory::InteropPhysRead(void* vmInstance, uintptr_t physAddress, void* buffer, size_t bufferSize) {
#ifdef _DEBUG
        FrameWork::Misc::Log(XorStr("InteropPhysRead chamado."));
#endif // DEBUG
        if (!vmInstancePtr) {
            vmInstancePtr = vmInstance;
#ifdef _DEBUG
            FrameWork::Misc::Log(XorStr("vmInstancePtr inicializado: "), vmInstancePtr);
#endif // DEBUG
        }
        return g_FreeFireContext.VirtualMemory.physRead(vmInstance, physAddress, buffer, bufferSize);
    }

    bool Memory::Interop() {

        FrameWork::Misc::Log(XorStr("Initializing Interop."));

        MH_Initialize();
        if (MH_CreateHook((LPVOID)g_FreeFireContext.VirtualMemory.physRead, InteropPhysRead, (LPVOID*)&g_FreeFireContext.VirtualMemory.tempPhysRead) != MH_OK) {
            FrameWork::Misc::Log(XorStr("Erro ao criar hook para physRead."));
        }
        if (MH_EnableHook((LPVOID)g_FreeFireContext.VirtualMemory.physRead) != MH_OK) {
            FrameWork::Misc::Log(XorStr("Erro ao habilitar hook para physRead."));
        }

        FrameWork::Misc::Log(XorStr("Waiting VM Instance Ptr...."));

        int timeout = 5000;
        int elapsed = 0;

        while (vmInstancePtr == nullptr && elapsed < timeout) {
            Sleep(1);
            elapsed++;
        }

        if (vmInstancePtr == nullptr) {
            FrameWork::Misc::Log(XorStr("Erro: Tempo limite atingido ao esperar por VM Instance Ptr."));
            return false;
        }

        if (MH_DisableHook((LPVOID)g_FreeFireContext.VirtualMemory.physRead) != MH_OK) {
            FrameWork::Misc::Log(XorStr("Erro ao desabilitar o hook em physRead."));
            return false;
        }

        FrameWork::Misc::Log(XorStr("Ambos os hooks foram desabilitados com sucesso."));
        return vmInstancePtr != nullptr;
    }

    bool Memory::Convert(uintptr_t address, uintptr_t& phys) {
        phys = 0;
        auto it = g_FreeFireContext.VirtualMemory.cache.find(address);

        if (it != g_FreeFireContext.VirtualMemory.cache.end()) {
            phys = it->second;
            return true;
        }

        for (int i = 0; i < 4; ++i) {
            void* cpuAddr = g_FreeFireContext.VirtualMemory.physCpu(vmInstancePtr, i);

            if (cpuAddr && g_FreeFireContext.VirtualMemory.physConvert(cpuAddr, address, &phys) == 0) {
                g_FreeFireContext.VirtualMemory.cache[address] = phys;
                return true;
            }
        }
        return false;
    }
}