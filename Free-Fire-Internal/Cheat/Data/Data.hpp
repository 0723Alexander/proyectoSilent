#pragma once

#include <iostream>
#include <unordered_map>
#include <thread>
#include <string>
#include <FrameWork/Utilities/Memory/Memory.hpp>
#include <FrameWork/Math/Entity.hpp>
#include <FrameWork/Math/Transform.hpp>
#include <Cheat/Offsets/Offsets.hpp>
#include <Cheat/FreeFireSDK/Context.hpp>
#include <Cheat/Options.hpp>

inline float defaultRecoil = -1.0f;

namespace FrameWork {
    class Data {
    public:
        static void Work();
    private:
        static void ClearGameState();
        static bool ParseEntity(uint32_t entityAddr, Entity& entity, Vector3& mainpos);
        static void HandleRecoil(uint32_t localPlayer);
        static void ReadAndStorePlayerName(uintptr_t entityAddr, Entity& entity);
        static void LoadBones(uintptr_t entityAddr, Entity& entity, Vector3& mainPos);
 
    };
}
