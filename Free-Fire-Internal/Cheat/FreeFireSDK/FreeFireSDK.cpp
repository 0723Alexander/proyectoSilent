#include <Cheat/FreeFireSDK/FreeFireSDK.hpp>
#include <FrameWork/Math/Vectors/Vector3.hpp>
#include <FrameWork/Math/Vectors/Vector4.hpp>

namespace Cheat {
    bool FreeFireSDK::Setup() {

#ifdef _DEBUG
        FrameWork::Misc::Log(XorStr("[FreeFire-Cheat] Starting FreeFireSDK Setup..."));
#endif 
        if (g_FreeFireContext.SDKReady) {
#ifdef _DEBUG
            FrameWork::Misc::Log(XorStr("SDK is already initialized."));
#endif
            return true;
        }

        HMODULE BlueStacksControllerModBase = SafeCall(GetModuleHandle)(XorStr(L"BstkVMM.dll"));
        if (!BlueStacksControllerModBase) {
#ifdef _DEBUG
            FrameWork::Misc::Log(XorStr("Failed to get handle for BstkVMM module."));
#endif
            return false;
        }

#ifdef _DEBUG
        FrameWork::Misc::Log(XorStr("Handle to BstkVMM.dll successfully obtained."));
#endif

        g_FreeFireContext.VirtualMemory.physRead = reinterpret_cast<VirtualMemoryFunctions::PGMPhysReadType>(
            SafeCall(GetProcAddress)(BlueStacksControllerModBase, XorStr("PGMPhysRead")));
        g_FreeFireContext.VirtualMemory.physWrite = reinterpret_cast<VirtualMemoryFunctions::PGMPhysSimpleWriteGCPhysType>(
            SafeCall(GetProcAddress)(BlueStacksControllerModBase, XorStr("PGMPhysSimpleWriteGCPhys")));
        g_FreeFireContext.VirtualMemory.physCpu = reinterpret_cast<VirtualMemoryFunctions::VMMGetCpuByIdType>(
            SafeCall(GetProcAddress)(BlueStacksControllerModBase, XorStr("VMMGetCpuById")));
        g_FreeFireContext.VirtualMemory.physConvert = reinterpret_cast<VirtualMemoryFunctions::PGMPhysGCPtr2GCPhysType>(
            SafeCall(GetProcAddress)(BlueStacksControllerModBase, XorStr("PGMPhysGCPtr2GCPhys")));

#ifdef _DEBUG
        FrameWork::Misc::Log(XorStr("Loaded function pointers."));
#endif
        if (!g_FreeFireContext.VirtualMemory.physRead ||
            !g_FreeFireContext.VirtualMemory.physWrite ||
            !g_FreeFireContext.VirtualMemory.physCpu ||
            !g_FreeFireContext.VirtualMemory.physConvert) {
#ifdef _DEBUG
            FrameWork::Misc::Log(XorStr("Failed to retrieve necessary function pointers."));
#endif
            return false;
        }

#ifdef _DEBUG
        FrameWork::Misc::Log(XorStr("Interop initialization started."));
#endif

        if (!FrameWork::g_FreeFireMemory.Interop()) {
#ifdef _DEBUG
            FrameWork::Misc::Log(XorStr("Interop initialization failed."));
#endif
            return false;
        }

        g_FreeFireContext.SDKReady = true;

#ifdef _DEBUG
        FrameWork::Misc::Log(XorStr("[FreeFire-Cheat] SDK successfully configured and ready to use."));
#endif
        return true;
    }
}