#include "Aimbot.hpp"
#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include <FrameWork/Math/Quaternion.hpp>

namespace FrameWork {
    void AimBot::Work() {
        while (!g_Options.General.ShutDown) {
            if (!g_Options.LegitBot.AimBot.Enabled) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }

            if ((GetAsyncKeyState(g_Options.LegitBot.AimBot.KeyBind) & 0x8000) == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }

            Entity* target = nullptr;
            float closestDistance = FLT_MAX;

            if (Context::WindowWidth == 0 || Context::WindowHeight == 0) continue;
            if (!Context::HasMatrix) continue;

            Vector2 screenCenter(Context::WindowWidth / 2.0f, Context::WindowHeight / 2.0f);

            for (auto& pair : Context::Entities) {
                Entity& entity = pair.second;

                if (!entity.IsKnown || entity.IsDead || g_Options.LegitBot.AimBot.IgnoreKnocked && entity.IsKnocked)
                    continue;

                auto Head = W2S::WorldToScreen(Context::ViewMatrix, entity.Head, Context::WindowWidth, Context::WindowHeight);
                if (Head.X < 0 || Head.Y < 0) continue;

                float playerDistance = Vector3::Distance(Context::LocalMainCamera, entity.Head);
                if (playerDistance > g_Options.LegitBot.AimBot.MaxDistance) continue;

                float x = Head.X - screenCenter.X;
                float y = Head.Y - screenCenter.Y;
                float crosshairDist = std::sqrt(x * x + y * y);

                if (crosshairDist < closestDistance && crosshairDist < g_Options.LegitBot.AimBot.FOV) {
                    closestDistance = crosshairDist;
                    target = &entity;
                }
            }

            if (target != nullptr) {
                auto playerLook = Quaternion::GetRotationToLocation(target->Head, 0.1f, Context::LocalMainCamera);
                FrameWork::Memory::WriteToMemory(Context::LocalPlayerAddress + Offsets::AimRotation, playerLook);

            }

            /*
            if (target != nullptr) {
                Vector3 aim_pos = target->Head;

                aim_pos.Y += 0.15f;

                auto playerLook = Quaternion::GetRotationToLocation(aim_pos, 0.0f, Context::LocalMainCamera);
                FrameWork::Memory::WriteToMemory(Context::LocalPlayerAddress + Offsets::AimRotation, playerLook);
            }*/



            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    };
};