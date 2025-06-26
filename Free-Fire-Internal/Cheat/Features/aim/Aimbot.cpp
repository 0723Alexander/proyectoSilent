#include "Aimbot.hpp"
#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include <FrameWork/Math/Quaternion.hpp>

namespace FrameWork {
    void AimBot::Work() {
        while (!g_Options.General.ShutDown) {

            // =========================================================================
            // ===== INICIO DE LA LÓGICA CORREGIDA =====================================
            // =========================================================================

            Entity* best_target = nullptr; // Variable para guardar el mejor objetivo del ciclo actual

            // PASO 1: Si el aimbot está activado en el menú, buscamos SIEMPRE al mejor objetivo.
            if (g_Options.LegitBot.AimBot.Enabled) {

                float closestDistance = FLT_MAX;

                if (Context::WindowWidth > 0 && Context::WindowHeight > 0 && Context::HasMatrix) {
                    Vector2 screenCenter(Context::WindowWidth / 2.0f, Context::WindowHeight / 2.0f);

                    for (auto& pair : Context::Entities) {
                        Entity& entity = pair.second;

                        // Filtros para ignorar enemigos no válidos
                        if (!entity.IsKnown || entity.IsDead || (g_Options.LegitBot.AimBot.IgnoreKnocked && entity.IsKnocked))
                            continue;

                        auto Head = W2S::WorldToScreen(Context::ViewMatrix, entity.Head, Context::WindowWidth, Context::WindowHeight);
                        if (Head.X < 1 || Head.Y < 1) continue;

                        float playerDistance = Vector3::Distance(Context::LocalMainCamera, entity.Head);
                        if (playerDistance > g_Options.LegitBot.AimBot.MaxDistance) continue;

                        float x = Head.X - screenCenter.X;
                        float y = Head.Y - screenCenter.Y;
                        float crosshairDist = std::sqrt(x * x + y * y);

                        // Si este enemigo es mejor que el anterior, lo guardamos
                        if (crosshairDist < closestDistance && crosshairDist < g_Options.LegitBot.AimBot.FOV) {
                            closestDistance = crosshairDist;
                            best_target = &entity;
                        }
                    }
                }
            }

            // PASO 2: Actualizamos la variable global para la línea del ESP.
            // Esto se hace siempre, para que la línea se dibuje antes de presionar la tecla.
            Context::AimbotTarget = best_target;

            // PASO 3: El Aimbot SÓLO MUEVE LA MIRA si encontramos un objetivo Y la tecla está presionada.
            if (best_target != nullptr && (GetAsyncKeyState(g_Options.LegitBot.AimBot.KeyBind) & 0x8000))
            {
                // La lógica que realmente apunta y "dispara"
                auto playerLook = Quaternion::GetRotationToLocation(best_target->Head, 0.1f, Context::LocalMainCamera);
                FrameWork::Memory::WriteToMemory(Context::LocalPlayerAddress + Offsets::AimRotation, playerLook);

                // Pausa para dar ritmo al aimbot y no sobrecargar
                std::this_thread::sleep_for(std::chrono::milliseconds(15));
            }
            else
            {
                // Si el aimbot no está activo, hacemos una pausa mínima para ser eficientes
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            // =========================================================================
            // ===== FIN DE LA LÓGICA CORREGIDA ========================================
            // =========================================================================
        }
    };
};