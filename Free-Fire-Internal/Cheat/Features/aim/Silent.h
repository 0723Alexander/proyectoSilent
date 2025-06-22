#pragma once
#include "Aimbot.hpp"
#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include <FrameWork/Math/Quaternion.hpp>
#include <atomic>

namespace Silent_CppX {

    inline std::atomic<bool> cancelToken = false;
    inline std::atomic<bool> running = false;
    inline std::thread aimbotThread;

    inline void SilentAimAtTarget(Entity* target, const Vector3& shootOrigin, uint32_t boneBase) {
        if (!target || boneBase == 0) return;

        Vector3 targetHead = target->Head;
        targetHead.Y += 0.1f; // más agresivo, apunta ligeramente más arriba

        Vector3 direction = Vector3::Normalize(targetHead - shootOrigin);

        // Forzar dirección dos veces (más agresivo)
        FrameWork::Memory::WriteToMemory<Vector3>(boneBase + 0x2C, direction);
        FrameWork::Memory::WriteToMemory<Vector3>(boneBase + 0x2C, direction);
    }

    inline Entity* GetClosestTarget() {
        Entity* closestTarget = nullptr;
        float closestDist = FLT_MAX;

        for (auto& [id, entity] : Context::Entities) {
            if (!entity.IsKnown || entity.IsDead || (g_Options.LegitBot.AimBot.IgnoreKnocked && entity.IsKnocked))
                continue;

            float dist = Vector3::Distance(Context::LocalMainCamera, entity.Head);
            if (dist < closestDist && dist < g_Options.LegitBot.AimBot.MaxDistance) {
                closestDist = dist;
                closestTarget = &entity;
            }
        }

        return closestTarget;
    }

    inline std::vector<Entity*> FindTargetsInFov() {
        std::vector<Entity*> targets;

        for (auto& [id, entity] : Context::Entities) {
            if (!entity.IsKnown || entity.IsDead) continue;
            if (g_Options.LegitBot.AimBot.IgnoreKnocked && entity.IsKnocked) continue;

            float dist = Vector3::Distance(Context::LocalMainCamera, entity.Head);
            if (dist < g_Options.LegitBot.AimBot.MaxDistance) {
                targets.push_back(&entity);
            }
        }

        return targets;
    }

    inline void StartSilentAim() {
        if (running) return;
        cancelToken = false;
        running = true;

        aimbotThread = std::thread([] {
            while (!cancelToken.load()) {
                // Evita usar keybind: apunta siempre
                if (Context::WindowWidth <= 0 || Context::WindowHeight <= 0 || !Context::HasMatrix) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    continue;
                }

                uint32_t boneBase = FrameWork::Memory::Read<uint32_t>(Context::LocalPlayerAddress + 0x808);
                if (!boneBase) continue;

                Vector3 shootOrigin = FrameWork::Memory::Read<Vector3>(boneBase + 0x38);

                // 🔍 Buscar todos los objetivos en el FOV
                std::vector<Entity*> targets = FindTargetsInFov();
                for (Entity* target : targets) {
                    SilentAimAtTarget(target, shootOrigin, boneBase);
                }

                // 🔁 Ultra agresivo si está activado
                if (g_Options.LegitBot.AimBot.aggressiveMode) {
                    std::this_thread::yield();
                }
                else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }
            running = false;
            });
    }

    inline void StopSilentAim() {
        cancelToken = true;
        if (aimbotThread.joinable()) {
            aimbotThread.join();
        }
    }
}