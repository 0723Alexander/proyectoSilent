#include "Custom.h"
#include <imgui_internal.h>
#include <string>
#include <iostream>
#include <map>
#include <algorithm>
#include <FrameWork/Render/Assets.hpp>

using namespace ImGui;

namespace Colors {

    namespace Checkbox
    {
        ImVec4 BackgroundYellow = ImColor(255, 0, 255);  // Alterado de pink
        ImVec4 mark = ImColor(255, 255, 255);
        ImVec4 stroke = ImColor(28, 26, 37);
        ImVec4 background = ImColor(15, 15, 15, 130);
        ImVec4 background_widget = ImColor(0.09f, 0.09f, 0.09f, 1.f);
        ImVec4 text_active = ImColor(255, 255, 255);
        ImVec4 inactive_color = ImColor(200, 200, 200);
        ImVec4 label_inactive_color = ImColor(156, 156, 156);
        ImVec4 description_active_color = ImColor(0.6f, 0.6f, 0.6f, 1.f);
        ImVec4 description_inactive_color = ImColor(47, 47, 47);

        float rounding = 5;
    }

    namespace Slider
    {
        ImVec4 BackgroundPink = ImColor(255, 0, 255);
        ImVec4 background = ImColor(15, 15, 15, 130);
        ImVec4 text_active = ImColor(255, 255, 255);
        ImVec4 inactive_color = ImColor(200, 200, 200);
        ImVec4 label_inactive_color = ImColor(156, 156, 156);
        ImVec4 description_active_color = ImColor(0.6f, 0.6f, 0.6f, 1.f);
        ImVec4 description_inactive_color = ImColor(47, 47, 47);
        ImVec4 second_color(0.09f, 0.09f, 0.09f, 1.f);
        float rounding = 5;
    }

    namespace BeginChild {

        ImVec4 accent_color = ImColor(255, 0, 255);
        ImVec4 accent_color22 = ImColor(15, 15, 15);
        ImVec4 accent_color222 = ImColor(225, 225, 225);
        ImVec4 child_background = ImColor(18, 18, 18, 255);
        ImVec4 border_child = ImColor(28, 27, 32, 255);
        ImVec4 child_gradient = ImColor(75, 89, 153, 255);
        ImVec4 border_child_default = ImColor(22, 21, 26, 255);
        ImVec4 child_name = ImColor(62, 61, 65, 255);
        ImVec4 text_active = ImColor(255, 255, 255, 255);
        ImVec4 tab_push_gradient = ImColor(79, 102, 209, 255);
    }
    
    namespace Tab_SubTab {

        ImVec4 main_color = ImColor(255, 0, 255);
    }
}

template <typename T>
T clamp(T val, T min, T max) {
    return (val < min) ? min : (val > max) ? max : val;
}
inline float anim_speed = 8.f;
inline float pos_offset;
inline bool size_change;

namespace Custom {

    void RenderTextColor(ImFont* font, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, const char* text, const ImVec2& align)
    {
        PushFont(font);
        PushStyleColor(ImGuiCol_Text, col);
        RenderTextClipped(p_min, p_max, text, NULL, NULL, align, NULL);
        PopStyleColor();
        PopFont();
    }


    bool Checkbox2(const char* label, const char* description, bool* v)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems) return false;

        struct check_state
        {
            ImVec4 label_color;
            ImVec4 description_color;
            ImVec4 text_color;
            ImVec4 background, text;
            float alpha, mark_pos;
        };

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const float w = GetContentRegionMax().x - style.WindowPadding.x;
        const float square_sz = 20.f;
        const ImVec2 pos = window->DC.CursorPos;
        const ImRect rect(pos, pos + ImVec2(w, 50));

        ItemSize(rect, 0.f);
        if (!ItemAdd(rect, id)) return false;

        bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held);
        if (IsItemClicked())
        {
            *v = !(*v);
            MarkItemEdited(id);
        }

        static std::map<ImGuiID, check_state> anim;

        if (anim.find(id) == anim.end())
        {
            anim[id] = {};
            anim[id].background = Colors::Checkbox::background_widget;
        }
        check_state& state = anim[id];

        state.background = ImLerp(state.background, *v ? Colors::Checkbox::BackgroundYellow : Colors::Checkbox::background_widget, g.IO.DeltaTime * 6.f);
        state.alpha = ImLerp(state.alpha, *v ? 1.f : 0.f, g.IO.DeltaTime * 6.f);

        ImVec4 active_color = Colors::Checkbox::text_active;
        ImVec4 inactive_color = Colors::Checkbox::inactive_color;
        state.text_color = ImLerp(state.text_color, *v ? active_color : inactive_color, g.IO.DeltaTime * 6.f);

        ImVec4 label_active_color = Colors::Checkbox::text_active;
        ImVec4 label_inactive_color = Colors::Checkbox::label_inactive_color;
        ImVec4 description_active_color = Colors::Checkbox::description_active_color;
        ImVec4 description_inactive_color = Colors::Checkbox::description_inactive_color;

        state.label_color = ImLerp(state.label_color, *v ? label_active_color : label_inactive_color, g.IO.DeltaTime * 6.f);
        state.description_color = ImLerp(state.description_color, *v ? description_active_color : description_inactive_color, g.IO.DeltaTime * 6.f);

        state.mark_pos = ImClamp(state.mark_pos + (35.f * g.IO.DeltaTime * (*v ? -3.f : 3.f)), 0.f, 35.f);

        const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));


        RenderTextColor(FrameWork::Assets::InterRegular, rect.Min + ImVec2(10, 0), rect.Max, GetColorU32(state.description_color), description, ImVec2(0.0, 0.8));


        GetWindowDrawList()->AddRect(rect.Max - ImVec2(37, 37), rect.Max - ImVec2(13, 13), GetColorU32(state.background), 5.f);
        GetWindowDrawList()->AddRectFilled(rect.Max - ImVec2(37, 37), rect.Max - ImVec2(13, 13), GetColorU322(state.background, 5.f), 2.f);

        PushClipRect(rect.Max - ImVec2(37, 37), rect.Max - ImVec2(13, 13), true);
        RenderCheckMark(GetWindowDrawList(), rect.Max - ImVec2(35 - (square_sz / 2) / 2, 35 - (square_sz / 2 + state.mark_pos) / 2), GetColorU322(Colors::Checkbox::mark, state.alpha), square_sz / 2);
        PopClipRect();


        RenderTextColor(FrameWork::Assets::InterRegular, rect.Min + ImVec2(10, 0), rect.Max, GetColorU32(state.label_color), label, ImVec2(0.0, 0.2));

        return pressed;
    }

    bool Checkbox3(const char* label, bool* v, float fixed_width)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems) return false;

        struct check_state
        {
            ImVec4 label_color;
            ImVec4 description_color;
            ImVec4 text_color;
            ImVec4 background, text, b;
            float alpha, mark_pos;
        };

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        const float square_sz = 22.0f;
        const ImVec2 pos = window->DC.CursorPos;
        const float total_height = square_sz;
        const float total_width = fixed_width;
       /* const ImRect rect(pos, pos + ImVec2(total_width, total_height));*/
        const ImRect rect(pos, pos + ImVec2(GetWindowSize().x - 5, total_height));

        ItemSize(rect, 4.0f);
        if (!ItemAdd(rect, id)) return false;

        bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held);
        if (IsItemClicked())
        {
            *v = !(*v);
            MarkItemEdited(id);
        }

        static std::map<ImGuiID, check_state> anim;
        auto it_anim = anim.find(id);

        if (it_anim == anim.end())
        {
            anim.insert({ id, check_state() });
            anim[id] = {};
            anim[id].background = Colors::Checkbox::background_widget;
            anim[id].b = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
            it_anim = anim.find(id);
        }
        check_state& state = anim[id];

        state.background = ImLerp(state.background, *v ? Colors::Checkbox::BackgroundYellow : Colors::Checkbox::background_widget, g.IO.DeltaTime * 6.f);
        state.b = ImLerp(state.b, hovered ? Colors::Checkbox::BackgroundYellow : ImVec4(0.0f, 0.0f, 0.0f, 0.0f), g.IO.DeltaTime * 10.f);

        state.alpha = ImLerp(state.alpha, *v ? 1.f : 0.f, g.IO.DeltaTime * 6.f);

        if (*v)
            state.mark_pos = ImLerp(state.mark_pos, 0.f, g.IO.DeltaTime * 6.f);
        else
            state.mark_pos = ImLerp(state.mark_pos, square_sz - 6.f, g.IO.DeltaTime * 6.f);

        ImVec4 active_color = Colors::Checkbox::text_active;
        ImVec4 inactive_color = Colors::Checkbox::inactive_color;
        state.text_color = ImLerp(state.text_color, *v ? active_color : inactive_color, g.IO.DeltaTime * 6.f);

        ImVec4 label_active_color = Colors::Checkbox::text_active;
        ImVec4 label_inactive_color = Colors::Checkbox::label_inactive_color;
        state.label_color = ImLerp(state.label_color, *v ? label_active_color : label_inactive_color, g.IO.DeltaTime * 6.f);

        const ImRect check_bb(rect.Max - ImVec2(square_sz + 15.0f, square_sz), rect.Max - ImVec2(15.0f, 0.0f));

        float corner_radius = square_sz * 0.3f;
        ImDrawList* draw_list = GetWindowDrawList();

        draw_list->AddRect(check_bb.Min, check_bb.Max,
            GetColorU32(state.background), corner_radius);

        draw_list->AddRectFilled(check_bb.Min, check_bb.Max,
            GetColorU32(state.background), corner_radius);

        ImU32 border_color = ImGui::GetColorU32(state.b);
        const float border_thickness = 0.1f;
        draw_list->AddRect(check_bb.Min, check_bb.Max, border_color, corner_radius, 0, border_thickness);

        PushClipRect(check_bb.Min, check_bb.Max, true);
        RenderCheckMark(
            draw_list,
            ImVec2(
                check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - (square_sz - 8) / 2,
                check_bb.Min.y + (check_bb.Max.y - check_bb.Min.y) / 2 - (square_sz - 8) / 2)
            + ImVec2(0, state.mark_pos),
            GetColorU322(Colors::Checkbox::mark, state.alpha),
            square_sz - 8);
        PopClipRect();

        ImVec2 label_offset = ImVec2(pos.x + -4.0f, pos.y + (total_height - g.FontSize) / 2.0f);
        RenderTextColor(FrameWork::Assets::InterRegular, label_offset, rect.Max, GetColorU32(state.label_color), label, ImVec2(0.0f, 0.5f));

        return pressed;
    }


    //bool Checkbox3(const char* label, const char* description, bool* v, float fixed_width)
    //{
    //    ImGuiWindow* window = GetCurrentWindow();
    //    if (window->SkipItems) return false;

    //    struct check_state
    //    {
    //        ImVec4 label_color;
    //        ImVec4 description_color;
    //        ImVec4 text_color;
    //        ImVec4 background, text;
    //        float alpha, mark_pos;
    //    };

    //    ImGuiContext& g = *GImGui;
    //    const ImGuiStyle& style = g.Style;
    //    const ImGuiID id = window->GetID(label);

    //    const float square_sz = 20.0f;
    //    const ImVec2 pos = window->DC.CursorPos;
    //    const float total_height = 50.0f;
    //    const float total_width = fixed_width;
    //    const ImRect rect(pos, pos + ImVec2(total_width, total_height));

    //    ItemSize(rect, 0.f);
    //    if (!ItemAdd(rect, id)) return false;

    //    bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held);
    //    if (IsItemClicked())
    //    {
    //        *v = !(*v);
    //        MarkItemEdited(id);
    //    }

    //    static std::map<ImGuiID, check_state> anim;

    //    if (anim.find(id) == anim.end())
    //    {
    //        anim[id] = {};
    //        anim[id].background = Colors::Checkbox::background_widget;
    //    }
    //    check_state& state = anim[id];

    //    state.background = ImLerp(state.background, *v ? Colors::Checkbox::BackgroundPink : Colors::Checkbox::background_widget, g.IO.DeltaTime * 6.f);
    //    state.alpha = ImLerp(state.alpha, *v ? 1.f : 0.f, g.IO.DeltaTime * 6.f);

    //    ImVec4 active_color = Colors::Checkbox::text_active;
    //    ImVec4 inactive_color = Colors::Checkbox::inactive_color;
    //    state.text_color = ImLerp(state.text_color, *v ? active_color : inactive_color, g.IO.DeltaTime * 6.f);

    //    ImVec4 label_active_color = Colors::Checkbox::text_active;
    //    ImVec4 label_inactive_color = Colors::Checkbox::label_inactive_color;
    //    ImVec4 description_active_color = Colors::Checkbox::description_active_color;
    //    ImVec4 description_inactive_color = Colors::Checkbox::description_inactive_color;

    //    state.label_color = ImLerp(state.label_color, *v ? label_active_color : label_inactive_color, g.IO.DeltaTime * 6.f);
    //    state.description_color = ImLerp(state.description_color, *v ? description_active_color : description_inactive_color, g.IO.DeltaTime * 6.f);

    //    state.mark_pos = ImClamp(state.mark_pos + (35.f * g.IO.DeltaTime * (*v ? -3.f : 3.f)), 0.f, 35.f);

    //    const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));

    //    // Descrição com maior deslocamento
    //    RenderTextColor(FrameWork::Assets::InterRegular14, rect.Min + ImVec2(-5, 0), rect.Max, GetColorU32(state.description_color), description, ImVec2(0.0, 0.8));

    //    //background
    //   // GetWindowDrawList()->AddRect(rect.Max - ImVec2(37, 37), rect.Max - ImVec2(13, 13), GetColorU32(state.background), 5.f);
    //   // GetWindowDrawList()->AddRectFilled(rect.Max - ImVec2(37, 37), rect.Max - ImVec2(13, 13), GetColorU322(state.background, 5.f), 2.f);

    //    GetWindowDrawList()->AddRect(rect.Max - ImVec2(37, 37), rect.Max - ImVec2(13, 13), GetColorU32(state.background), 6.f);
    //    GetWindowDrawList()->AddRectFilled(rect.Max - ImVec2(37, 37), rect.Max - ImVec2(13, 13), GetColorU322(state.background, 5.f), 5.f);

    //    PushClipRect(rect.Max - ImVec2(37, 37), rect.Max - ImVec2(13, 13), true);
    //    RenderCheckMark(GetWindowDrawList(), rect.Max - ImVec2(35 - (square_sz / 2) / 2, 35 - (square_sz / 2 + state.mark_pos) / 2), GetColorU322(Colors::Checkbox::mark, state.alpha), square_sz / 2);
    //    PopClipRect();

    //    // Label com menor deslocamento
    //    RenderTextColor(FrameWork::Assets::InterRegular14, rect.Min + ImVec2(-5, 0), rect.Max, GetColorU32(state.label_color), label, ImVec2(0.0, 0.2));

    //    return pressed;
    //}

    bool SliderScalar2(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        struct slider_state {
            ImVec4 text_color;
            ImVec4 grab_color;
            float grab_offset;
            ImVec4 label_color;
            ImVec4 description_color;
        };

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        const float available_width = GetContentRegionAvail().x;
        const ImVec2 frame_size = ImVec2(available_width, 30.0f);

        const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + frame_size);

        const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
        ItemSize(frame_bb, style.FramePadding.y);
        ItemAdd(frame_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0);

        static std::map<ImGuiID, slider_state> anim;
        auto it_anim = anim.find(id);

        if (it_anim == anim.end())
        {
            slider_state initial_state;
            initial_state.grab_color = Colors::Slider::BackgroundPink;
            initial_state.label_color = Colors::Slider::label_inactive_color;
            initial_state.description_color = Colors::Slider::description_inactive_color;
            initial_state.text_color = Colors::Slider::inactive_color;
            const float range = *(float*)p_max - *(float*)p_min;
            const float normalized_value = (*(float*)p_data - *(float*)p_min) / range;
            initial_state.grab_offset = normalized_value * (available_width - 20);

            anim[id] = initial_state;
            it_anim = anim.find(id);
        }

        const ImVec2 slider_start_offset = ImVec2(-3, 12);
        const ImRect slider_bb(frame_bb.Min + slider_start_offset, ImVec2(frame_bb.Min.x + (available_width - 20) + 11, frame_bb.Min.y + 24));

        if (format == NULL)
            format = DataTypeGetInfo(data_type)->PrintFmt;

        const bool hovered = ItemHoverable(frame_bb, id, 0);
        bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
        if (!temp_input_is_active)
        {
            const bool input_requested_by_tabbing = temp_input_allowed && (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
            const bool clicked = hovered && IsMouseClicked2(0, id);
            const bool make_active = (input_requested_by_tabbing || clicked || g.NavActivateId == id || g.NavActivateInputId == id);
            if (make_active && clicked)
                SetKeyOwner(ImGuiKey_MouseLeft, id);
            if (make_active && temp_input_allowed)
                if (input_requested_by_tabbing || (clicked && g.IO.KeyCtrl) || g.NavActivateInputId == id)
                    temp_input_is_active = true;

            if (make_active && !temp_input_is_active)
            {
                SetActiveID(id, window);
                SetFocusID(id, window);
                FocusWindow(window);
                g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
            }
        }

        ImRect grab_bb;
        const bool value_changed = SliderBehavior(ImRect(slider_bb.Min - ImVec2(5, 0), slider_bb.Max + ImVec2(5, 0)), id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
        if (value_changed)
            MarkItemEdited(id);

        float current_value = 0.0f;
        float min_value = 0.0f;

        if (data_type == ImGuiDataType_Float)
        {
            current_value = *(float*)p_data;
            min_value = *(float*)p_min;
        }
        else if (data_type == ImGuiDataType_S32)
        {
            current_value = (float)*(int*)p_data;
            min_value = (float)*(int*)p_min;
        }

        ImVec4 active_color = Colors::Slider::text_active;
        ImVec4 inactive_color = Colors::Slider::inactive_color;

        it_anim->second.grab_offset = ImLerp(it_anim->second.grab_offset, (grab_bb.Min.x + (grab_bb.Max.x - grab_bb.Min.x) / 2) - slider_bb.Min.x, g.IO.DeltaTime * 6.0f);
        it_anim->second.grab_color = ImLerp(it_anim->second.grab_color, Colors::Slider::BackgroundPink, g.IO.DeltaTime * 6.0f);
        it_anim->second.text_color = ImLerp(it_anim->second.text_color, current_value == min_value ? inactive_color : active_color, g.IO.DeltaTime * 6.0f);

        ImVec4 label_active_color = Colors::Slider::text_active;
        ImVec4 label_inactive_color = Colors::Slider::label_inactive_color;

        it_anim->second.label_color = ImLerp(it_anim->second.label_color, current_value == min_value ? label_inactive_color : label_active_color, g.IO.DeltaTime * 6.0f);

        RenderTextColor(
            FrameWork::Assets::InterRegular14,
            frame_bb.Min + ImVec2(-3, -7),
            frame_bb.Max,
            GetColorU32(it_anim->second.label_color),
            label,
            ImVec2(0.0, 0.2)
        );

        window->DrawList->AddRectFilled(slider_bb.Min, slider_bb.Max, GetColorU32(Colors::Slider::second_color), 25);

        float grab_offset_clamped = ImClamp(it_anim->second.grab_offset, 3.0f, (available_width - 20) - 3.0f + 11.0f);
        window->DrawList->AddRectFilled(slider_bb.Min, slider_bb.Min + ImVec2(grab_offset_clamped, slider_bb.Max.y - slider_bb.Min.y), GetColorU32(Colors::Slider::BackgroundPink), 25);

        float grab_width = 8.0f;
        float grab_height = 15.0f;
        float grab_rounding = 4.0f;

        ImVec2 grab_min = ImVec2(
            slider_bb.Min.x + grab_offset_clamped - grab_width / 2,
            slider_bb.Min.y + (slider_bb.Max.y - slider_bb.Min.y) / 2 - grab_height / 2
        );
        ImVec2 grab_max = ImVec2(
            grab_min.x + grab_width,
            grab_min.y + grab_height
        );

        window->DrawList->AddRectFilled(grab_min, grab_max, GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), grab_rounding);

        char value_buf[64];
        const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);
        ImVec2 value_size = CalcTextSize(value_buf);

        float value_x = frame_bb.Max.x - (-5 + value_size.x) - 2;
        if (value_x + value_size.x > frame_bb.Max.x - 3) {
            value_x = frame_bb.Max.x - value_size.x - 8 - 1;
        }

        window->DrawList->AddText(ImVec2(value_x, frame_bb.Min.y + 5 - value_size.y / 2), ImColor(1.f, 1.f, 1.f, 1.f), value_buf);

        return value_changed;
    }

    //// Desenhar o "corpinho branco" do slider (grab)
//ImVec2 grab_center = ImVec2(
//    slider_bb.Min.x + it_anim->second.grab_offset,
//    (slider_bb.Min.y + slider_bb.Max.y) * 0.5f
//);
//float grab_radius = 10.0f; // Tamanho do "corpinho branco"
//window->DrawList->AddCircleFilled(grab_center, grab_radius, GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), 32);


    // Renderizar o slider
    //float height_reduction = 115.0f;
    //ImVec2 new_min = frame_bb.Min + ImVec2(0.0f, height_reduction / 2.0f);
    //ImVec2 new_max = frame_bb.Max - ImVec2(0.0f, height_reduction / 2.0f);
    //window->DrawList->AddRectFilled(new_min, new_max, GetColorU32(Colors::Slider::background), Colors::Slider::rounding);
    //window->DrawList->AddRect(new_min, new_max, GetColorU32(Colors::Slider::background), style.FrameRounding);

    bool SliderFloat2(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags, const char* hint)
    {
        return SliderScalar2(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
    }

    bool SliderInt(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags, const char* hint)
    {
        return SliderScalar2(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
    }

    bool BeginChildPosEx(const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags flags)
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* parent_window = g.CurrentWindow;

        flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_ChildWindow;
        flags |= (parent_window->Flags & ImGuiWindowFlags_NoMove);

        // Size
        const ImVec2 content_avail = GetContentRegionAvail();
        ImVec2 size = ImFloor(size_arg);
        const int auto_fit_axises = ((size.x == 0.0f) ? (1 << ImGuiAxis_X) : 0x00) | ((size.y == 0.0f) ? (1 << ImGuiAxis_Y) : 0x00);
        if (size.x <= 0.0f)
            size.x = ImMax(content_avail.x + size.x, 4.0f); // Arbitrary minimum child size (0.0f causing too many issues)
        if (size.y <= 0.0f)
            size.y = ImMax(content_avail.y + size.y, 4.0f);
        SetNextWindowSize(size);


        ImVec2 rect_start = parent_window->DC.CursorPos;
        ImVec2 rect_end = rect_start + size;

        // Build up name. If you need to append to a same child from multiple location in the ID stack, use BeginChild(ImGuiID id) with a stable value.
        const char* temp_window_name;
        if (name)
            ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s_%08X", parent_window->Name, name, id);
        else
            ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%08X", parent_window->Name, id);

        if (!border) {

            ImGui::GetWindowDrawList()->AddRectFilled(parent_window->DC.CursorPos + ImVec2(0.0f, 0.0f), parent_window->DC.CursorPos + ImVec2(size_arg.x + 0.1f, size_arg.y), GetColorU32(Colors::BeginChild::child_background), g.Style.ChildRounding + 2, ImGuiWindowFlags_None);
 
            ImGui::GetWindowDrawList()->AddRect(parent_window->DC.CursorPos + ImVec2(1, 0.0f), parent_window->DC.CursorPos + ImVec2(size_arg.x - 1, size_arg.y - 1), GetColorU322(Colors::BeginChild::accent_color222, 0.0f), g.Style.ChildRounding, ImGuiWindowFlags_None, 0);

            // Gradiente superior (centralizado e fino)
            ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
                parent_window->DC.CursorPos + ImVec2(1, 0.0f),
                parent_window->DC.CursorPos + ImVec2(size_arg.x / 2, 2),
                GetColorU322(Colors::BeginChild::accent_color, 0),
                GetColorU32(Colors::BeginChild::accent_color),
                GetColorU32(Colors::BeginChild::accent_color),
                GetColorU322(Colors::BeginChild::accent_color, 0)
            );

            ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
                parent_window->DC.CursorPos + ImVec2(size_arg.x - 1, 0.0f),
                parent_window->DC.CursorPos + ImVec2(size_arg.x / 2, 2),
                GetColorU322(Colors::BeginChild::accent_color, 0),
                GetColorU32(Colors::BeginChild::accent_color),
                GetColorU32(Colors::BeginChild::accent_color),
                GetColorU322(Colors::BeginChild::accent_color, 0)
            );

            //// Gradiente inferior (centralizado e fino)
            //ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
            //    parent_window->DC.CursorPos + ImVec2(1, size_arg.y - 2),
            //    parent_window->DC.CursorPos + ImVec2(size_arg.x / 2, size_arg.y),
            //    GetColorU322(Colors::BeginChild::accent_color, 0),
            //    GetColorU32(Colors::BeginChild::accent_color),
            //    GetColorU32(Colors::BeginChild::accent_color),
            //    GetColorU322(Colors::BeginChild::accent_color, 0)
            //);

            //ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
            //    parent_window->DC.CursorPos + ImVec2(size_arg.x - 1, size_arg.y - 2),
            //    parent_window->DC.CursorPos + ImVec2(size_arg.x / 2, size_arg.y),
            //    GetColorU322(Colors::BeginChild::accent_color, 0),
            //    GetColorU32(Colors::BeginChild::accent_color),
            //    GetColorU32(Colors::BeginChild::accent_color),
            //    GetColorU322(Colors::BeginChild::accent_color, 0)
            //);

            ImVec2 title_rect_min = rect_start + ImVec2(1.0f, 1.5f);
            ImVec2 title_rect_max = rect_start + ImVec2(size.x - 1.0f, 40.0f);
            ImGui::GetWindowDrawList()->AddRectFilled(
                title_rect_min,
                title_rect_max,
                GetColorU32(Colors::BeginChild::accent_color22),
                g.Style.ChildRounding,
                ImDrawFlags_RoundCornersTop
            );

            ImGui::GetWindowDrawList()->AddText(
                FrameWork::Assets::InterBold,
                20.f,
                parent_window->DC.CursorPos + ImVec2(10, 10),
                GetColorU32(Colors::BeginChild::child_name),
                name
            );

            ImGui::GetWindowDrawList()->AddLine(
                rect_start + ImVec2(1.0f, 40.0f),
                rect_start + ImVec2(size.x - 1.0f, 40.0f),
                GetColorU32(Colors::BeginChild::border_child),
                1.5f
            );

        }

        bool ret = Begin(temp_window_name, NULL, flags);

        ImGuiWindow* child_window = g.CurrentWindow;
        child_window->ChildId = id;
        child_window->AutoFitChildAxises = (ImS8)auto_fit_axises;

        if (child_window->BeginCount == 1)
            parent_window->DC.CursorPos = child_window->Pos;

        // Process navigation-in immediately so NavInit can run on first frame
        if (g.NavActivateId == id && !(flags & ImGuiWindowFlags_NavFlattened) && (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY))
        {
            FocusWindow(child_window);
            NavInitWindow(child_window, false);
            SetActiveID(id + 1, child_window); // Steal ActiveId with another arbitrary id so that key-press won't activate child item
            g.ActiveIdSource = ImGuiInputSource_Nav;
        }
        return ret;
    }

    bool BeginChildPos(const char* str_id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags)
    {
        ImGuiWindow* window = GetCurrentWindow();
        PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 50));
        PushStyleVar(ImGuiStyleVar_ChildRounding, 5);
        //PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 8));
        return BeginChildPosEx(str_id, window->GetID(str_id), size_arg, border, extra_flags | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar);
    }

    void EndChildPos()
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        PopStyleVar(2);
        IM_ASSERT(g.WithinEndChild == false);
        IM_ASSERT(window->Flags & ImGuiWindowFlags_ChildWindow);   // Mismatched BeginChild()/EndChild() calls

        g.WithinEndChild = true;
        if (window->BeginCount > 1)
        {
            End();
        }
        else
        {
            ImVec2 sz = window->Size;
            if (window->AutoFitChildAxises & (1 << ImGuiAxis_X)) // Arbitrary minimum zero-ish child size of 4.0f causes less trouble than a 0.0f
                sz.x = ImMax(4.0f, sz.x);
            if (window->AutoFitChildAxises & (1 << ImGuiAxis_Y))
                sz.y = ImMax(4.0f, sz.y);
            End();

            ImGuiWindow* parent_window = g.CurrentWindow;
            ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + sz);
            ItemSize(sz);
            if ((window->DC.NavLayersActiveMask != 0 || window->DC.NavWindowHasScrollY) && !(window->Flags & ImGuiWindowFlags_NavFlattened))
            {
                ItemAdd(bb, window->ChildId);
                RenderNavHighlight(bb, window->ChildId);

                // When browsing a window that has no activable items (scroll only) we keep a highlight on the child (pass g.NavId to trick into always displaying)
                if (window->DC.NavLayersActiveMask == 0 && window == g.NavWindow)
                    RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, ImGuiNavHighlightFlags_TypeThin);
            }
            else
            {
                // Not navigable into
                ItemAdd(bb, 0);
            }
            if (g.HoveredWindow == window)
                g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;
        }
        g.WithinEndChild = false;
        g.LogLinePosY = -FLT_MAX; // To enforce a carriage return

        //ImGui::Spacing();
    }

    bool SubTab(const char* label, int* tab, int number)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        const float square_sz = GetFrameHeight();
        const ImVec2 pos = window->DC.CursorPos;
        const ImRect total_bb(pos, pos + ImVec2(100, 70));
        ItemSize(total_bb, style.FramePadding.y);
        if (!ItemAdd(total_bb, id))
        {
            IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
            return false;
        }

        static std::map<ImGuiID, tab_state> anim;
        auto it_anim = anim.find(id);

        if (it_anim == anim.end())
        {
            anim.insert({ id, tab_state() });
            it_anim = anim.find(id);
        }

        bool hovered, held;
        bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
        if (pressed)
        {
            size_change = true;
            *tab = number;
            MarkItemEdited(id);
        }

        it_anim->second.line_offset = ImLerp(it_anim->second.line_offset, *tab == number ? 3.f : 0.f, g.IO.DeltaTime * anim_speed);
        it_anim->second.text_color = ImLerp(it_anim->second.text_color, *tab == number ? ImColor(1.f, 1.f, 1.f, 1.f) : ImColor(0.4f, 0.4f, 0.4f, 1.f), g.IO.DeltaTime * anim_speed);

        window->DrawList->AddRectFilled(total_bb.Min - ImVec2(-10, 2), total_bb.Min + ImVec2(90, it_anim->second.line_offset), GetColorU32(Colors::Tab_SubTab::main_color), 90.f, ImDrawFlags_RoundCornersBottom);

        window->DrawList->AddText(total_bb.Min + ImVec2(50 - CalcTextSize(label).x / 2, 35 - CalcTextSize(label).y / 2), GetColorU32(it_anim->second.text_color), label);

        return pressed;
    }

    bool Tab(const char* icon, int* tab, int number)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(icon);

        const float square_sz = GetFrameHeight();
        const ImVec2 pos = window->DC.CursorPos;
        const ImRect total_bb(pos, pos + ImVec2(80, 50));
        ItemSize(total_bb, style.FramePadding.y);
        if (!ItemAdd(total_bb, id))
        {
            IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
            return false;
        }

        static std::map<ImGuiID, tab_state> anim;
        auto it_anim = anim.find(id);

        if (it_anim == anim.end())
        {
            anim.insert({ id, tab_state() });
            it_anim = anim.find(id);
        }

        bool hovered, held;
        bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
        if (pressed)
        {
            size_change = true;
            *tab = number;
            MarkItemEdited(id);
        }


        it_anim->second.line_offset = ImLerp(it_anim->second.line_offset, *tab == number ? 3.f : 0.f, g.IO.DeltaTime * anim_speed);
        it_anim->second.text_color = ImLerp(it_anim->second.text_color, *tab == number ? ImColor(1.f, 1.f, 1.f, 1.f) : ImColor(0.4f, 0.4f, 0.4f, 1.f), g.IO.DeltaTime * anim_speed);

        window->DrawList->AddRectFilled(total_bb.Min - ImVec2(2, 0), total_bb.Min + ImVec2(it_anim->second.line_offset, 50.f), GetColorU32(Colors::Tab_SubTab::main_color), 90.f, ImDrawFlags_RoundCornersRight);

        PushFont(FrameWork::Assets::FontAwesomeSolid);
        window->DrawList->AddText(total_bb.Min + ImVec2(40 - CalcTextSize(icon).x / 2, 25 - CalcTextSize(icon).y / 2), GetColorU32(it_anim->second.text_color), icon);
        PopFont();

        Spacing();
        //Spacing();

        return pressed;
    }
}
