#include <Cheat/Cheat.hpp>
#include "Features/aim/AimBot.hpp"
#include "Features/aim/Silent.h"



// Assuming 'font' is a namespace with 'lexend_bold' as a font pointer
namespace font {
	ImFont* lexend_bold;
}

// Ensure 'io' is defined properly
ImGuiIO& io = ImGui::GetIO();  // Access ImGui's IO object
namespace Cheat {
	DWORD WINAPI Unload() {
		if (g_pInterface) {
			g_pInterface->ShutDown();
			delete g_pInterface;
			g_pInterface = nullptr;
		}

		FrameWork::Misc::ShutDownConsole();
		if (g_hModule) {
			FreeLibraryAndExitThread(g_hModule, 0);
		}
		return 0;
	}

	void PushFont(ImFont* font) {
		ImGui::PushFont(font);  // ImGui tem essa função nativa
	}

	void Initialize() {
		FrameWork::Misc::Log(XorStr("[FreeFire-Cheat] Initializing..."));

		while (!g_FreeFireContext.SDKReady) {
			FrameWork::Misc::Log(XorStr("[FreeFire-Cheat] Waiting for FreeFire SDK setup..."));

			Cheat::g_FreeFire.Setup();

			if (!g_FreeFireContext.SDKReady) {
				std::this_thread::sleep_for(std::chrono::seconds(5));
			}
		}

		FrameWork::Overlay::Setup(FrameWork::FindeRender::FindRenderWindow());
		FrameWork::Overlay::Initialize();
		FrameWork::Misc::Log(XorStr("[FreeFire-Cheat] Initializing adb..."));
		std::thread([&]() { FrameWork::ADB::InitializeADB(); }).detach();
		FrameWork::Misc::Log(XorStr("[FreeFire-Cheat] Initializing Data..."));
		std::thread([&]() { FrameWork::Data::Work(); }).detach();
		std::thread([&]() { FrameWork::AimBot::Work(); }).detach();
		FrameWork::Misc::Log(XorStr("[FreeFire-Cheat] Initialization complete."));

		if (FrameWork::Overlay::IsInitialized())
		{
			FrameWork::Interface Interface(FrameWork::Overlay::GetOverlayWindow(), FrameWork::Overlay::GetTargetWindow(), FrameWork::Overlay::dxGetDevice(), FrameWork::Overlay::dxGetDeviceContext());
			Interface.UpdateStyle();
			FrameWork::Overlay::SetupWindowProcHook(std::bind(&FrameWork::Interface::WindowProc, &Interface, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

			MSG Message;
			ZeroMemory(&Message, sizeof(Message));
			while (Message.message != WM_QUIT)
			{
				HWND hWindow = FrameWork::Overlay::GetOverlayWindow();
				if (hWindow == nullptr) {
					FrameWork::Misc::Log(XorStr("Overlay window handle é nullptr."));
					break;
				}

				if (PeekMessage(&Message, hWindow, NULL, NULL, PM_REMOVE))
				{
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}

				InvalidateRect(hWindow, NULL, TRUE);
				UpdateWindow(hWindow);

				ImGui::GetIO().MouseDrawCursor = Interface.GetMenuOpen();
			
				if (Interface.ResizeHeight != 0 || Interface.ResizeWidht != 0)
				{
					FrameWork::Overlay::dxCleanupRenderTarget();
					FrameWork::Overlay::dxGetSwapChain()->ResizeBuffers(0, Interface.ResizeWidht, Interface.ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
					Interface.ResizeHeight = Interface.ResizeWidht = 0;
					FrameWork::Overlay::dxCreateRenderTarget();
				}

				Interface.HandleMenuKey();
				FrameWork::Overlay::UpdateWindowPos();

				static bool CaptureBypassOn = false;
				if (g_Options.General.CaptureBypass != CaptureBypassOn)
				{
					CaptureBypassOn = g_Options.General.CaptureBypass;
					SafeCall(SetWindowDisplayAffinity)(FrameWork::Overlay::GetOverlayWindow(), CaptureBypassOn ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
				}

				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();
				{
					if (g_Options.Misc.Screen.ShowAimbotFov) {
						ImColor Color = ImColor(g_Options.Misc.Screen.AimbotFovColor[0], g_Options.Misc.Screen.AimbotFovColor[1], g_Options.Misc.Screen.AimbotFovColor[2], g_Options.Misc.Screen.AimbotFovColor[3]);
						ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), g_Options.LegitBot.AimBot.FOV, Color, 360);
					}

					if (g_Options.Visuals.ESP.Players.Enabled) {
						ESP::Players();
					}

					if (g_Options.LegitBot.AimBot.Silent) {
						g_Options.Misc.Screen.ShowAimbotFov = true;

						Silent_CppX::StartSilentAim();

					}
					else {
						g_Options.Misc.Screen.ShowAimbotFov = false;

						Silent_CppX::StopSilentAim();
					}

					Interface.RenderGui();
				}
				ImGui::EndFrame();
				ImGui::Render();

				FrameWork::Overlay::dxRefresh();
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
				FrameWork::Overlay::dxGetSwapChain()->Present(0, 0);

				if (g_Options.General.ShutDown) {
					Unload();
					return;
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(g_Options.General.ThreadDelay));
			}
		}
	}
}