#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <Windows.h>
#include <unordered_map>
#include <FrameWork/Math/Matrix4x4.hpp>
#include <FrameWork/Math/Entity.hpp>
#include <FrameWork/Math/Vectors/Vector3.hpp>

class Context {
public:
    static inline uint32_t LibraryAddress = 0;
    static inline bool HasMatrix = false;
    static inline Matrix4x4 ViewMatrix{};
    static inline std::unordered_map<uint32_t, Entity> Entities;
    static inline Vector3 LocalMainCamera{};
    static inline uint32_t LocalPlayerAddress = 0;
    static inline int WindowWidth = 0;
    static inline int WindowHeight = 0;
};

struct VirtualMemoryFunctions {
    using PGMPhysReadType = int(__cdecl*)(void*, uintptr_t, void*, size_t);
    using PGMPhysSimpleWriteGCPhysType = int(__cdecl*)(void*, uintptr_t, void*, size_t);
    using PGMPhysGCPtr2GCPhysType = int(__cdecl*)(void*, uintptr_t, uintptr_t*);
    using VMMGetCpuByIdType = void* (__cdecl*)(void*, int);

    static inline PGMPhysReadType physRead = nullptr;
    static inline PGMPhysReadType tempPhysRead = nullptr;
    static inline PGMPhysSimpleWriteGCPhysType physWrite = nullptr;
    static inline PGMPhysGCPtr2GCPhysType physConvert = nullptr;
    static inline VMMGetCpuByIdType physCpu = nullptr;
    static inline std::unordered_map<uintptr_t, uintptr_t> cache;
};

struct FreeFireContext {
    bool SDKReady = false;
    static inline VirtualMemoryFunctions VirtualMemory;
};

inline FreeFireContext g_FreeFireContext;

#endif
