#pragma once
#include <imgui.h>
#include <imgui_internal.h>
#include <Windows.h>

namespace Custom {
	void RenderTextColor(ImFont* font, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, const char* text, const ImVec2& align);
	bool Checkbox2(const char* label, const char* description, bool* v);
	bool Checkbox3(const char* label, bool* v, float fixed_width = 200.0f);
	bool SliderFloat2(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0, const char* hint = "");
	bool SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0, const char* hint = "");
	bool BeginChildPosEx(const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags flags);
	bool BeginChildPos(const char* str_id, const ImVec2& size_arg, bool border = false, ImGuiWindowFlags extra_flags = 0);
	void EndChildPos();
	struct tab_state {
		ImVec4 background_color;
		ImVec4 text_color;
		float line_offset = 0.0f;
	};
	bool Tab(const char* icon, int* tab, int number);
	bool SubTab(const char* label, int* tab, int number);
}