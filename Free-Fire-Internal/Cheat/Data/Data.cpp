#include "Data.hpp"

namespace FrameWork {
    void Data::Work() {
        std::cout << "Thread Started" << std::endl;

        uint32_t previousCount = 0;

        while (!g_Options.General.ShutDown) {

            std::this_thread::sleep_for(std::chrono::milliseconds(5));

            Context::HasMatrix = false;

            uint32_t baseFacade = Memory::Read<uint32_t>(Context::LibraryAddress + Offsets::InitBase);
            if (!baseFacade) {
                ClearGameState();
                continue;
            }

            uint32_t gameFacade = Memory::Read<uint32_t>(baseFacade);
            if (!gameFacade) {
                ClearGameState();
                continue;
            }

            uint32_t staticGameFacade = Memory::Read<uint32_t>(gameFacade + Offsets::StaticClass);
            if (!staticGameFacade) {
                ClearGameState();
                continue;
            }

            uint32_t currentGame = Memory::Read<uint32_t>(staticGameFacade);
            if (!currentGame) {
                ClearGameState();
                continue;
            }

            uint32_t currentMatch = Memory::Read<uint32_t>(currentGame + Offsets::CurrentMatch);
            if (!currentMatch) {
                ClearGameState();
                continue;
            }

            uint32_t matchStatus = Memory::Read<uint32_t>(currentMatch + Offsets::MatchStatus);
            if (!matchStatus) continue;
            if (matchStatus != 1) {
                ClearGameState();
                continue;
            }

            defaultRecoil = -1.0f;

            uint32_t localPlayer = Memory::Read<uint32_t>(currentMatch + Offsets::LocalPlayer);
            if (!localPlayer) {
                ClearGameState();
                continue;
            }

            HandleRecoil(localPlayer);
            Context::LocalPlayerAddress = localPlayer;

            uint32_t mainTransform = Memory::Read<uint32_t>(localPlayer + Offsets::MainCameraTransform);
            if (!mainTransform) {
                ClearGameState();
                continue;
            }

            Vector3 mainPos;
            Transform::GetPosition(mainTransform, mainPos);

            Context::LocalMainCamera = mainPos;

            uint32_t followCamera = Memory::Read<uint32_t>(localPlayer + Offsets::FollowCamera);
            if (!followCamera) {
                ClearGameState();
                continue;
            }

            uint32_t camera = Memory::Read<uint32_t>(followCamera + Offsets::Camera);
            if (!camera) {
                ClearGameState();
                continue;
            }

            uint32_t cameraBase = Memory::Read<uint32_t>(camera + 0x8);
            if (!cameraBase) {
                ClearGameState();
                continue;
            }

            Matrix4x4 viewMatrix = Memory::Read<Matrix4x4>(cameraBase + Offsets::ViewMatrix);
            Context::HasMatrix = true;
            Context::ViewMatrix = viewMatrix;

            uint32_t entityDictionary = Memory::Read<uint32_t>(currentGame + Offsets::DictionaryEntities);
            if (!entityDictionary) {
                ClearGameState();
                continue;
            }

            uint32_t entities = Memory::Read<uint32_t>(entityDictionary + 0x14);
            if (!entities)
            {
                ClearGameState();
                continue;
            }

            entities += 0x10;

            uint32_t entitiesCount = Memory::Read<uint32_t>(entityDictionary + 0x18);
            if (entitiesCount != previousCount) {
                std::cout << "Entities Count: " << entitiesCount << std::endl;
                previousCount = entitiesCount;
            }

            if (entitiesCount < 1) continue;

            for (uint32_t i = 0; i < entitiesCount; i++) {
                uint32_t entityAddr = Memory::Read<uint32_t>(entities + i * 0x4);

                if (!entityAddr || entityAddr == localPlayer) continue;

                Entity entity;
                auto entry = Context::Entities.find(entityAddr);
                if (entry != Context::Entities.end()) {
                    ParseEntity(entityAddr, entry->second, mainPos);
                }
                else {
                    Entity newEntity;
                    if (ParseEntity(entityAddr, newEntity, mainPos)) {
                        Context::Entities[entityAddr] = newEntity;
                    }
                }
            }
        }
    }

    bool Data::ParseEntity(uint32_t entityAddr, Entity& entity, Vector3& mainPos) {
        uint32_t avatarManager;
        if (!FrameWork::Memory::Read1(entityAddr + Offsets::AvatarManager, avatarManager) || avatarManager == 0) {
            Context::Entities.erase(entityAddr);
            return false;
        }

        uint32_t avatar;
        if (!FrameWork::Memory::Read1(avatarManager + Offsets::Avatar, avatar) || avatar == 0) {
            Context::Entities.erase(entityAddr);
            return false;
        }

        /*bool isVisible = false;
        if (!Memory::Read1(avatar + Offsets::Avatar_IsVisible, isVisible) || !isVisible) {
            Context::Entities.erase(entityAddr);
            return false;
        }*/

        bool isGameVisible = false;
        Memory::Read1(avatar + Offsets::Avatar_IsVisible, isGameVisible);

        // 2. Comprueba si debemos saltar la entidad
        //    Saltaremos la entidad SOLO SI:
        //    a) NO es visible en el juego, Y
        //    b) Nuestra opción para ver a los invisibles está DESACTIVADA.
        if (!isGameVisible && !g_Options.Visuals.ESP.Players.VisibleOnly) {
            Context::Entities.erase(entityAddr);
            return false;
        }

        // 3. Si llegamos aquí, es porque la entidad es visible o porque forzamos que se vea.
        //    Así que la marcamos como visible para el resto del código.
        entity.IsVisible = true;

        uint32_t avatarData;
        if (!FrameWork::Memory::Read1(avatar + Offsets::Avatar_Data, avatarData) || avatarData == 0) {
            Context::Entities.erase(entityAddr);
            return false;
        }

        bool isTeam = false;
        if (FrameWork::Memory::Read1<bool>(avatarData + Offsets::Avatar_Data_IsTeam, isTeam)) {
            entity.IsTeam = isTeam ? Bool3::True : Bool3::False;
            entity.IsKnown = !isTeam;
        }

        if (entity.IsTeam == Bool3::True) {
            return false;
        }

        if (!entity.IsKnown) return false;

        uint32_t shadowBase = 0;
        int xpose = 0;
        if (FrameWork::Memory::Read1<uint32_t>(entityAddr + Offsets::Player_ShadowBase, shadowBase) && shadowBase != 0) {
            {
                if (FrameWork::Memory::Read1<int>(shadowBase + Offsets::XPose, xpose))
                {
                    entity.IsKnocked = xpose == 8;
                }
            }
        }

        bool isDead = FrameWork::Memory::Read<bool>(entityAddr + Offsets::Player_IsDead);
        entity.IsDead = isDead;

        uint32_t dataPool = FrameWork::Memory::Read<uint32_t>(entityAddr + Offsets::Player_Data);
        if (dataPool) {
            uint32_t poolObj = FrameWork::Memory::Read<uint32_t>(dataPool + 0x8);
            if (poolObj) {
                uint32_t pool = FrameWork::Memory::Read<uint32_t>(poolObj + 0x10);
                if (pool) {
                    short health = FrameWork::Memory::Read<short>(pool + 0x10);
                    if (health != 0) {
                        entity.Health = health;
                    }
                }
            }
        }

        uint32_t headBone = 0;
        if (FrameWork::Memory::Read1<uint32_t>(entityAddr + static_cast<uint32_t>(Offsets::Bones::Head), headBone) && headBone != 0) {
            Vector3 headTransform = Vector3{ 0.0f, 0.0f, 0.0f };
            bool rHeadTrans = Transform::GetNodePosition(headBone, headTransform);
            if (rHeadTrans) {
                entity.Head = headTransform;
                entity.Distance = Vector3::Distance(mainPos, headTransform);
            }
        }

        uint32_t rootBone = 0;
        if (FrameWork::Memory::Read1<uint32_t>(entityAddr + static_cast<uint32_t>(Offsets::Bones::Root), rootBone) && rootBone != 0) {
            Vector3 rootTransform = Vector3{ 0.0f, 0.0f, 0.0f };
            bool rRootTrans = Transform::GetNodePosition(rootBone, rootTransform);
            if (rRootTrans) {
                entity.Root = rootTransform;
            }
        }

        LoadBones(entityAddr, entity, mainPos);

        ReadAndStorePlayerName(entityAddr, entity);

        return true;
    }

    void Data::LoadBones(uintptr_t entityAddr, Entity& entity, Vector3& mainPos) {
        struct BoneInfo {
            uintptr_t offset;
            Vector3* target;
        };

        std::vector<BoneInfo> bones = {
            {Offsets::Bones::Neck, &entity.Neck},
            {Offsets::Bones::OmbroRight, &entity.OmbroRight},
            {Offsets::Bones::OmbroLeft, &entity.OmbroLeft},
            {Offsets::Bones::RightCotovelo, &entity.RightCotovelo},
            {Offsets::Bones::LeftCotovelo, &entity.LeftCotovelo},
            {Offsets::Bones::RightPunho, &entity.RightPunho},
            {Offsets::Bones::LeftPunho, &entity.LeftPunho},
            {Offsets::Bones::Quadril, &entity.Quadril},
            {Offsets::Bones::Pinto, &entity.Pinto},
            {Offsets::Bones::RightTornozelo, &entity.RightTornozelo},
            {Offsets::Bones::LeftTornozelo, &entity.LeftTornozelo},
        };

        for (const auto& bone : bones) {
            uint32_t boneAddr = 0;
            if (FrameWork::Memory::Read1<uint32_t>(entityAddr + bone.offset, boneAddr) && boneAddr != 0) {
                Vector3 boneTransform = { 0.0f, 0.0f, 0.0f };
                if (Transform::GetNodePosition(boneAddr, boneTransform)) {
                    *bone.target = boneTransform;
                    if (bone.target == &entity.Head) {
                        entity.Distance = Vector3::Distance(mainPos, boneTransform);
                    }
                }
            }
        }
    }

    void Data::HandleRecoil(uint32_t localPlayer) {
        uint32_t weapon = FrameWork::Memory::Read<uint32_t>(localPlayer + Offsets::Weapon);
        if (weapon == 0) return;

        uint32_t weaponData = FrameWork::Memory::Read<uint32_t>(weapon + Offsets::WeaponData);
        if (weaponData == 0) return;

        float currentRecoil = FrameWork::Memory::Read<float>(weaponData + Offsets::WeaponRecoil);

        if (defaultRecoil < 0.0f) {
            defaultRecoil = currentRecoil;
        }

        if (g_Options.Misc.Exploits.LocalPlayer.norecoil) {
            if (currentRecoil != 0.0f) {
                FrameWork::Memory::WriteToMemory<float>(weaponData + Offsets::WeaponRecoil, 0.0f);
            }
        }
        else {
            if (currentRecoil != defaultRecoil) {
                FrameWork::Memory::WriteToMemory<float>(weaponData + Offsets::WeaponRecoil, defaultRecoil);
            }
        }
    }
    void Data::ReadAndStorePlayerName(uintptr_t entityAddr, Entity& entity) {
        uint32_t nameAddr = 0;

        if (FrameWork::Memory::Read1(entityAddr + Offsets::Player_Name, nameAddr) && nameAddr != 0) {
            const int nameLen = 256; 
            std::string name = FrameWork::Memory::ReadString(nameAddr + 0xC, nameLen);

          
            name.erase(std::remove_if(name.begin(), name.end(), [](unsigned char c) {
        
                return (!isprint(c) || (c < 0x20 || c > 0x7E)) && c != ' ' && c != '-';
                }), name.end());

            if (!name.empty()) {
                entity.Name = name;
            }
            else {
                entity.Name = "Bot"; 
            }
        }
        else {
            entity.Name = "Bot";
        }
    }

    void Data::ClearGameState() {
        if (!Context::Entities.empty()) {
            Context::Entities.clear();
        }

        if (Context::HasMatrix) {
            Context::HasMatrix = false;
            Context::ViewMatrix = Matrix4x4();
        }

        Context::LocalMainCamera = Vector3(0, 0, 0);
    }
};