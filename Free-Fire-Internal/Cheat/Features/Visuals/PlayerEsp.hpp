// Free-Fire-Internal/Cheat/Features/Visuals/PlayerEsp.hpp

#pragma once
#include "../../../FrameWork/Math/Vectors/Vector2.hpp"
#include "../../../FrameWork/Math/Vectors/Vector3.hpp"
#include "../../../FrameWork/Math/Matrix4x4.hpp" // For camera matrix
#include "../../../FrameWork/Render/Assets.hpp" // Potentially for color conversion or font data
#include "../../../FrameWork/Dependencies/ImGui/imgui.h" // ImGui headers
#include "../../../FrameWork/Math/W2S.hpp" // Your WorldToScreen function

// La clase Entity ya está en FrameWork/Math/Entity.hpp
// No se necesita una forward declaration aquí ya que Entity se incluye a través de Context.hpp
// que a su vez se incluye a través de FreeFireSDK.hpp

namespace Cheat
{
    namespace Features
    {
        namespace Visuals
        {
            class PlayerEsp
            {
            public:
                // Método principal para dibujar el ESP. Se llamaría en el bucle de renderizado.
                static void DrawPlayerESP();
                //static void DrawCompassESP(); // Función para la brújula ESP

                // Funciones de dibujo auxiliares
                static void DrawCorneredBox(float x, float y, float w, float h, ImU32 color, float thickness);
                static void DrawHealthBar(short health, short maxHealth, float x, float y, float height);
                static ImU32 ColorToImU32(float r, float g, float b, float a); // Convertir RGBA a ImU32
            };
        }
    }
}