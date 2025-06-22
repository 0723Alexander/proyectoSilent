#pragma once

// ImGui
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>

// FreeFireSDK
#include <Cheat/FreeFireSDK/FreeFireSDK.hpp>

// Features
#include <Cheat/Data/Data.hpp>
#include <Cheat/Features/Visuals/PlayerEsp.hpp>

#include <Cheat/Options.hpp>
#include <FrameWork/FrameWork.hpp>
extern HMODULE g_hModule;
extern FrameWork::Interface* g_pInterface;

namespace Cheat {
	void Initialize();
	DWORD WINAPI Unload();
	void ChamsContextShutdown(); // Declaração da funç
}