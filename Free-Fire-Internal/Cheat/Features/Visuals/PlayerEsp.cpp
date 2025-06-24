// Free-Fire-Internal/Cheat/Features/Visuals/PlayerEsp.cpp

#include "PlayerEsp.hpp"
#include "../../FreeFireSDK/FreeFireSDK.hpp" // Para acceder a los datos del juego (entidades, cámara)
#include "../../Data/Data.hpp" // Para las estructuras de datos del juego (si se usa directamente)
#include "../../Options.hpp" // Para tus opciones de ESP
#include "../../../FrameWork/Math/W2S.hpp" // Asegúrate de que tu W2S está implementado aquí
#include <cmath> // For std::abs, std::round, atan2, M_PI
#include <string> // For std::string usage, if needed for entity names etc.
#include <unordered_map> // Necesario para std::unordered_map si Context::Entities lo usa directamente aquí

// Acceso directo a miembros estáticos de Context (desde FreeFireSDK/Context.hpp)
// Acceso directo a la instancia global g_Options (desde Options.hpp)

namespace Cheat
{
    namespace Features
    {
        namespace Visuals
        {
            void PlayerEsp::DrawPlayerESP()
            {
                // Verifica si la matriz de la vista está disponible en Context
                if (!Context::HasMatrix) {
                    return;
                }

                // Obtener dimensiones de la pantalla del juego desde Context
                float screenWidth = static_cast<float>(Context::WindowWidth);
                float screenHeight = static_cast<float>(Context::WindowHeight);

                if (screenWidth <= 0 || screenHeight <= 0) {
                    return;
                }

                // Obtener la matriz de la cámara y la posición de la cámara local desde Context
                Matrix4x4 cameraMatrix = Context::ViewMatrix;
                Vector3 localMainCamera = Context::LocalMainCamera;

                // Iterar sobre todas las entidades (jugadores) del juego
                for (const auto& pair : Context::Entities)
                {
                    Entity entity = pair.second; // Tipo Entity de FrameWork/Math/Entity.hpp

                    // Filtrar entidades inválidas o muertas
                    if (entity.IsDead || !entity.IsKnown) {
                        continue;
                    }

                    // Calcular distancia entre el jugador local y la entidad
                    float dist = Vector3::Distance(localMainCamera, entity.Head);
                    if (dist > g_Options.Visuals.ESP.Players.RenderDistance) { // Usar opciones de RenderDistance
                        continue;
                    }

                    // Convertir posiciones 3D del mundo a coordenadas 2D de la pantalla
                    Vector2 headScreenPos = W2S::WorldToScreen(cameraMatrix, entity.Head, Context::WindowWidth, Context::WindowHeight);
                    Vector2 bottomScreenPos = W2S::WorldToScreen(cameraMatrix, entity.Root, Context::WindowWidth, Context::WindowHeight);

                    // Verificar si las coordenadas de pantalla son válidas y están dentro de los límites
                    // Se usan .X y .Y para acceder a los miembros del Vector2
                    //if (headScreenPos.X < 1.0f || headScreenPos.Y < 1.0f || bottomScreenPos.X < 1.0f || bottomScreenPos.Y < 1.0f ||
                    //    headScreenPos.X > screenWidth || headScreenPos.Y > screenHeight || bottomScreenPos.X > screenWidth || bottomScreenPos.Y > screenHeight)
                    //{
                    //    // La entidad no está visible en la pantalla principal (pero podría estarlo en la brújula)
                    //    continue; // No dibujamos el ESP normal si está fuera de pantalla
                    //}

                    // Calcular dimensiones de la caja del ESP
                    float cornerHeight = std::abs(headScreenPos.Y - bottomScreenPos.Y);
                    float cornerWidth = cornerHeight * 0.65f;

                    // Obtener la lista de dibujo de ImGui
                    ImDrawList* drawList = ImGui::GetForegroundDrawList();

                    // Dibujar línea (SnapLines)
                    if (g_Options.Visuals.ESP.Players.SnapLines)
                    {
                        drawList->AddLine(
                            ImVec2(screenWidth / 2.0f, 1.0f), // Centro superior de la pantalla
                            ImVec2(headScreenPos.X, headScreenPos.Y),
                            ColorToImU32(g_Options.Visuals.ESP.Players.SnapLinesColor[0], // Acceso a colores float[4]
                                g_Options.Visuals.ESP.Players.SnapLinesColor[1],
                                g_Options.Visuals.ESP.Players.SnapLinesColor[2],
                                g_Options.Visuals.ESP.Players.SnapLinesColor[3]),
                            1.0f
                        );
                    }

                    // Dibujar caja (ESPBox)
                    if (g_Options.Visuals.ESP.Players.Box)
                    {
                        ImU32 boxColor = ColorToImU32(g_Options.Visuals.ESP.Players.BoxColor[0],
                            g_Options.Visuals.ESP.Players.BoxColor[1],
                            g_Options.Visuals.ESP.Players.BoxColor[2],
                            g_Options.Visuals.ESP.Players.BoxColor[3]);
                        DrawCorneredBox(headScreenPos.X - (cornerWidth / 2.0f), headScreenPos.Y, cornerWidth, cornerHeight, boxColor, 1.0f);
                    }

                    // Dibujar nombre (ESPName) y distancia
                    //if (g_Options.Visuals.ESP.Players.Name)
                    //{
                    //    char nameBuffer[64];
                    //    // CORREGIDO: Se mantiene la lógica para incluir entity.Name
                    //    // Si los nombres no aparecen, el problema es que entity.Name no está siendo poblado por el SDK.
                    //    sprintf_s(nameBuffer, "%s [ %.0fM ]", entity.Name.c_str(), std::round(dist)); // entity.Name es std::string

                    //    drawList->AddText(
                    //        ImVec2(headScreenPos.X - (cornerWidth / 2.0f), headScreenPos.Y - 20.0f),
                    //        ColorToImU32(g_Options.Visuals.ESP.Players.NameColor[0],
                    //            g_Options.Visuals.ESP.Players.NameColor[1],
                    //            g_Options.Visuals.ESP.Players.NameColor[2],
                    //            g_Options.Visuals.ESP.Players.NameColor[3]),
                    //        nameBuffer
                    //    );
                    //}

                    // Dibujar barra de vida (ESPHealth)
                    if (g_Options.Visuals.ESP.Players.HealthBar)
                    {
                        DrawHealthBar(entity.Health, 200, headScreenPos.X - (cornerWidth / 2.0f) - 5.0f, headScreenPos.Y, cornerHeight);
                    }
                }
            }

            //void PlayerEsp::DrawCompassESP()
            //{
            //    // Verificar si la brújula está habilitada en las opciones
            //    if (!g_Options.Visuals.ESP.Players.Compass.Enabled) {
            //        return;
            //    }

            //    // Verificar si la matriz de la vista está disponible
            //    if (!Context::HasMatrix) {
            //        return;
            //    }

            //    // Obtener dimensiones de la pantalla
            //    float screenWidth = static_cast<float>(Context::WindowWidth);
            //    float screenHeight = static_cast<float>(Context::WindowHeight);

            //    // CORREGIDO: Centro del círculo de la brújula en la parte centro superior de la pantalla
            //    // Utilizamos el radio de la brújula para un pequeño offset del borde superior
            //    ImVec2 compassCenter = ImVec2(screenWidth / 2.0f, g_Options.Visuals.ESP.Players.Compass.Radius + 20.0f);
            //    float compassRadius = g_Options.Visuals.ESP.Players.Compass.Radius;
            //    float dotSize = g_Options.Visuals.ESP.Players.Compass.DotSize;

            //    ImDrawList* drawList = ImGui::GetForegroundDrawList();

            //    // Dibujar el círculo de la brújula
            //    drawList->AddCircle(compassCenter, compassRadius, ColorToImU32(g_Options.Visuals.ESP.Players.Compass.CircleColor[0], g_Options.Visuals.ESP.Players.Compass.CircleColor[1], g_Options.Visuals.ESP.Players.Compass.CircleColor[2], g_Options.Visuals.ESP.Players.Compass.CircleColor[3]), 0, 1.0f);

            //    // Obtener la posición de la cámara local y la matriz de vista desde Context
            //    Vector3 localMainCamera = Context::LocalMainCamera;
            //    Matrix4x4 viewMatrix = Context::ViewMatrix;

            //    // Extraer el vector 'hacia adelante' de la cámara desde la matriz de vista
            //    // Asumimos que la 3ª fila de la matriz de vista es el vector -Z de la cámara en el espacio mundial.
            //    Vector3 cameraForward = Vector3(-viewMatrix._31, -viewMatrix._32, -viewMatrix._33);
            //    cameraForward = Vector3::Normalize(cameraForward); // Normalizar el vector

            //    // Reducir el vector 'hacia adelante' a un vector 2D horizontal (ignorando el componente Y)
            //    Vector2 playerForward2D = Vector2::Normalized(Vector2(cameraForward.X, cameraForward.Z));

            //    // Iterar sobre las entidades para dibujar sus indicadores en la brújula
            //    for (const auto& pair : Context::Entities)
            //    {
            //        Entity entity = pair.second;

            //        // Filtrar entidades inválidas o muertas
            //        if (entity.IsDead || !entity.IsKnown) {
            //            continue;
            //        }

            //        // Opcional: Si el enemigo está en pantalla, no lo mostramos en la brújula
            //        Vector2 entityScreenPos = W2S::WorldToScreen(viewMatrix, entity.Head, Context::WindowWidth, Context::WindowHeight);
            //        if (entityScreenPos.X > 0 && entityScreenPos.X < screenWidth && entityScreenPos.Y > 0 && entityScreenPos.Y < screenHeight) {
            //            continue; // El enemigo ya está en pantalla, la brújula es para los que están fuera.
            //        }

            //        // Calcular el vector del jugador a la cabeza del enemigo (en el plano horizontal)
            //        Vector3 toEnemy3D = entity.Head - localMainCamera;
            //        Vector2 toEnemy2D = Vector2::Normalized(Vector2(toEnemy3D.X, toEnemy3D.Z));

            //        // Calcular el ángulo relativo entre la dirección del jugador y la dirección al enemigo
            //        // atan2(y, x) devuelve el ángulo en radianes.
            //        float angleToEnemy = std::atan2(toEnemy2D.Y, toEnemy2D.X) - std::atan2(playerForward2D.Y, playerForward2D.X);

            //        // Normalizar el ángulo a un rango de -PI a PI
            //        while (angleToEnemy > M_PI) angleToEnemy -= 2.0f * M_PI;
            //        while (angleToEnemy < -M_PI) angleToEnemy += 2.0f * M_PI;

            //        // Determinar si el enemigo está al frente o atrás usando el producto escalar 3D
            //        float dotProduct = Vector3::Dot(cameraForward, toEnemy3D);
            //        ImU32 indicatorColor = (dotProduct >= 0) // Si el producto escalar es positivo, está al frente
            //            ? ColorToImU32(g_Options.Visuals.ESP.Players.Compass.FrontColor[0], g_Options.Visuals.ESP.Players.Compass.FrontColor[1], g_Options.Visuals.ESP.Players.Compass.FrontColor[2], g_Options.Visuals.ESP.Players.Compass.FrontColor[3])
            //            : ColorToImU32(g_Options.Visuals.ESP.Players.Compass.BackColor[0], g_Options.Visuals.ESP.Players.Compass.BackColor[1], g_Options.Visuals.ESP.Players.Compass.BackColor[2], g_Options.Visuals.ESP.Players.Compass.BackColor[3]);

            //        // Calcular la posición del indicador en el círculo de la brújula
            //        // Multiplicamos por el radio para que esté en la circunferencia.
            //        float indicatorX = compassCenter.x + (compassRadius * std::cos(angleToEnemy));
            //        float indicatorY = compassCenter.y + (compassRadius * std::sin(angleToEnemy));

            //        // Dibujar el indicador (un círculo pequeño)
            //        drawList->AddCircleFilled(ImVec2(indicatorX, indicatorY), dotSize, indicatorColor);
            //    }
            //}


            void PlayerEsp::DrawCorneredBox(float x, float y, float w, float h, ImU32 color, float thickness)
            {
                ImDrawList* vList = ImGui::GetForegroundDrawList();

                float lineW = w / 4.0f;
                float lineH = h / 4.0f;

                // Top-left
                vList->AddLine(ImVec2(x, y - thickness / 3.0f), ImVec2(x, y + lineH), color, thickness);
                vList->AddLine(ImVec2(x - thickness / 3.0f, y), ImVec2(x + lineW, y), color, thickness);
                // Top-right
                vList->AddLine(ImVec2(x + w - lineW, y), ImVec2(x + w + thickness / 2.0f, y), color, thickness);
                vList->AddLine(ImVec2(x + w, y - thickness / 3.0f), ImVec2(x + w, y + lineH), color, thickness);
                // Bottom-left
                vList->AddLine(ImVec2(x, y + h - lineH), ImVec2(x, y + h + thickness / 2.0f), color, thickness);
                vList->AddLine(ImVec2(x - thickness / 3.0f, y + h), ImVec2(x + lineW, y + h), color, thickness);
                // Bottom-right
                vList->AddLine(ImVec2(x + w - lineW, y + h), ImVec2(x + w + thickness / 2.0f, y + h), color, thickness);
                vList->AddLine(ImVec2(x + w, y + h - lineH), ImVec2(x + w, y + h + thickness / 2.0f), color, thickness);
            }

            void PlayerEsp::DrawHealthBar(short health, short maxHealth, float x, float y, float height)
            {
                ImDrawList* vList = ImGui::GetForegroundDrawList();
                float healthPercentage = static_cast<float>(health) / static_cast<float>(maxHealth);
                float barHeight = height * healthPercentage;

                // Background bar (red)
                vList->AddRectFilled(ImVec2(x, y), ImVec2(x + 4, y + height), ColorToImU32(1.0f, 0.0f, 0.0f, 1.0f)); // Red
                // Foreground bar (green, based on health)
                vList->AddRectFilled(ImVec2(x, y + (height - barHeight)), ImVec2(x + 4, y + height), ColorToImU32(0.0f, 1.0f, 0.0f, 1.0f)); // Green
            }

            ImU32 PlayerEsp::ColorToImU32(float r, float g, float b, float a)
            {
                return ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, a));
            }
        }
    }
}