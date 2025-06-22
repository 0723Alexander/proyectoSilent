#include <imgui.h>
#include <cmath>
#include <iostream>
#include <Cheat/Options.hpp>
#include <Cheat/Features/Visuals/PlayerEsp.hpp>
#include <Cheat/FreeFireSDK/Context.hpp>
#include <FrameWork/Math/Entity.hpp>
#include <FrameWork/Math/Vectors/Vector2.hpp>
#include <FrameWork/Math/W2S.hpp>
bool IsTeam = false;
namespace font
{
    inline ImFont* WeaponsIco = nullptr;

    inline ImFont* icomoon = nullptr;
    inline ImFont* lexend_bold = nullptr;
    inline ImFont* lexend_regular = nullptr;
    inline ImFont* lexend_general_bold = nullptr;

    inline ImFont* icomoon_widget = nullptr;
    inline ImFont* icomoon_widget2 = nullptr;

}

namespace Cheat {

    uint32_t ColorToUint32(const ImVec4& color) {
        return ImColor(color.x * 255, color.y * 255, color.z * 255, color.w * 255);
    }

    uint32_t LerpColor(uint32_t color1, uint32_t color2, float t)
    {
        uint8_t a1 = (color1 >> 24) & 0xFF;
        uint8_t r1 = (color1 >> 16) & 0xFF;
        uint8_t g1 = (color1 >> 8) & 0xFF;
        uint8_t b1 = color1 & 0xFF;

        uint8_t a2 = (color2 >> 24) & 0xFF;
        uint8_t r2 = (color2 >> 16) & 0xFF;
        uint8_t g2 = (color2 >> 8) & 0xFF;
        uint8_t b2 = color2 & 0xFF;

        uint8_t a = static_cast<uint8_t>(a1 + (a2 - a1) * t);
        uint8_t r = static_cast<uint8_t>(r1 + (r2 - r1) * t);
        uint8_t g = static_cast<uint8_t>(g1 + (g2 - g1) * t);
        uint8_t b = static_cast<uint8_t>(b1 + (b2 - b1) * t);

        return (a << 24) | (r << 16) | (g << 8) | b;
    }


    void ESP::Players() {
        for (auto& [entityID, entity] : Context::Entities) {

            if (entity.IsDead || !entity.IsKnown) continue;

            float dist = Vector3::Distance(Context::LocalMainCamera, entity.Head);
            if (dist > g_Options.Visuals.ESP.Players.RenderDistance) continue;

            Vector2 headPos = W2S::WorldToScreen(Context::ViewMatrix, entity.Head, Context::WindowWidth, Context::WindowHeight);
            Vector2 rootPos = W2S::WorldToScreen(Context::ViewMatrix, entity.Root, Context::WindowWidth, Context::WindowHeight);

            if (headPos.X < 0 || headPos.Y < 0 || rootPos.X < 0 || rootPos.Y < 0) continue;

            float boxHeight = fabsf(headPos.Y - rootPos.Y);
            float boxWidth = boxHeight * 0.65f;
        }
    }
}
