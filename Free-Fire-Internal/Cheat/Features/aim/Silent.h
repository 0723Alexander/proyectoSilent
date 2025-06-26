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
                if (Context::WindowWidth <= 0 || Context::WindowHeight <= 0 || !Context::HasMatrix) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    continue;
                }

                uint32_t boneBase = FrameWork::Memory::Read<uint32_t>(Context::LocalPlayerAddress + 0x808);
                if (!boneBase) continue;

                Vector3 shootOrigin = FrameWork::Memory::Read<Vector3>(boneBase + 0x38);

                std::vector<Entity*> possible_targets = FindTargetsInFov();

                // =========================================================================
                // ===== INICIO DE LA SOLUCIÓN AL PARPADEO =================================
                // =========================================================================

                if (!possible_targets.empty()) {
                    Entity* best_target = nullptr;
                    float closest_distance_to_crosshair = FLT_MAX;
                    Vector2 screen_center(Context::WindowWidth / 2.0f, Context::WindowHeight / 2.0f);

                    for (Entity* current_target : possible_targets) {
                        if (!current_target) continue;
                        Vector2 head_on_screen = W2S::WorldToScreen(Context::ViewMatrix, current_target->Head, Context::WindowWidth, Context::WindowHeight);
                        float distance_to_crosshair = Vector2::Distance(screen_center, head_on_screen);
                        if (distance_to_crosshair < closest_distance_to_crosshair) {
                            closest_distance_to_crosshair = distance_to_crosshair;
                            best_target = current_target;
                        }
                    }

                    if (best_target != nullptr) {
                        // Si encontramos un objetivo, lo publicamos y apuntamos.
                        // La variable se mantendrá estable mientras se apunte al mismo objetivo.
                        Context::SilentAimTarget = best_target;
                        SilentAimAtTarget(best_target, shootOrigin, boneBase);
                    }
                    else {
                        // Si después de buscar no encontramos a nadie, AHORA SÍ limpiamos el objetivo.
                        Context::SilentAimTarget = nullptr;
                    }
                }
                else {
                    // Si la lista inicial de objetivos está vacía, también limpiamos el objetivo.
                    Context::SilentAimTarget = nullptr;
                }

                // =========================================================================
                // ===== FIN DE LA SOLUCIÓN =================================================
                // =========================================================================

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