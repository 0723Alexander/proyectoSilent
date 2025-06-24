#include <FrameWork/Render/interface.hpp>
#include <Cheat/Options.hpp>
#include <Cheat/Cheat.hpp>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

std::string logMessage = "";

void SetProcessPriority(HANDLE hProcess, DWORD priorityClass) {
	if (!SetPriorityClass(hProcess, priorityClass)) {
		MessageBox(NULL, L"Falha ao ajustar a prioridade do processo.", L"Erro", MB_OK | MB_ICONERROR);
	}
}

std::string GetProcessPriority(HANDLE hProcess) {
	DWORD priority = GetPriorityClass(hProcess);

	switch (priority) {
	case IDLE_PRIORITY_CLASS: return "Idle";
	case BELOW_NORMAL_PRIORITY_CLASS: return "Below Normal";
	case NORMAL_PRIORITY_CLASS: return "Normal";
	case ABOVE_NORMAL_PRIORITY_CLASS: return "Above Normal";
	case HIGH_PRIORITY_CLASS: return "High";
	case REALTIME_PRIORITY_CLASS: return "Realtime";
	default: return "Desconhecida";
	}
}

namespace FrameWork {

	std::string Interface::GetLogMessage() {
		return logMessage;
	}

	void Interface::SetLogMessage(const std::string& message) {
		logMessage = message;
	}

	void Interface::Initialize(HWND Window, HWND TargetWindow, ID3D11Device* Device, ID3D11DeviceContext* DeviceContext) {
		hWindow = Window;
		IDevice = Device;

		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hWindow);
		ImGui_ImplDX11_Init(Device, DeviceContext);

		InitializeMenu();
	}

	void Interface::InitializeMenu() {
		bIsMenuOpen = true;
		SetWindowLong(hWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
		SetForegroundWindow(hWindow);
		SetWindowPos(hWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
	}

	void Interface::UpdateStyle() {
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle* Style = &ImGui::GetStyle();
		Style->AntiAliasedLines = true;
		Style->AntiAliasedLinesUseTex = true;
		Style->AntiAliasedFill = true;
		Style->WindowRounding = 5;
		Style->FrameRounding = 3.f;
		Style->ItemSpacing = ImVec2(0, 3);
		Style->WindowBorderSize = 0;
		Style->WindowPadding = ImVec2(0, 0);
		Style->ScrollbarSize = 1;
		Style->FramePadding.y = 2.0f;
		Style->ScrollbarRounding = 5;
		Style->PopupRounding = 5;


		Style->Colors[ImGuiCol_Separator] = ImColor(30, 144, 255, 255); 
		Style->Colors[ImGuiCol_SeparatorActive] = ImColor(30, 144, 255, 255); 
		Style->Colors[ImGuiCol_SeparatorHovered] = ImColor(30, 144, 255, 255);
		Style->Colors[ImGuiCol_ResizeGrip] = ImColor(30, 144, 255, 255); 
		Style->Colors[ImGuiCol_ResizeGripActive] = ImColor(30, 144, 255, 255);
		Style->Colors[ImGuiCol_ResizeGripHovered] = ImColor(30, 144, 255, 255); 


		Style->Colors[ImGuiCol_ScrollbarGrab] = ImColor(30, 144, 255, 255);
		Style->Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(30, 144, 255, 255);
		Style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(30, 144, 255, 255);
		Style->Colors[ImGuiCol_WindowBg] = ImColor(12, 10, 12);
		Style->Colors[ImGuiCol_Border] = ImColor(24, 23, 25); 
		Style->Colors[ImGuiCol_PopupBg] = ImColor(14, 14, 14);
		Assets::Initialize(IDevice);
	}



	void Interface::AimbotTab() {
		ImGui::SetCursorPos(ImVec2(20, 10));
		ImGui::SetWindowFontScale(1.2f);
		ImGui::Text(XorStr("Aim Assist"));
		ImGui::SetWindowFontScale(0.9f);

		ImGui::SetCursorPos(ImVec2(20, 26));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::Text(XorStr("Improve your aim in game"));
		ImGui::PopFont();
		ImGui::PopStyleColor();
		ImGui::SetWindowFontScale(1.0f);

		ImGui::SetCursorPos(ImVec2(20, 60));
		ImGui::CustomChild(XorStr("Aimbot"), ImVec2(ImGui::GetWindowSize().x / 2 - 30, 350));
		{
			Custom::Checkbox3("Aimbot", &g_Options.LegitBot.AimBot.Enabled);

			Custom::Checkbox3(XorStr("Enabled"), &g_Options.LegitBot.AimBot.Silent, 235.0f);
			Custom::Checkbox3(XorStr("Show Fov"),&g_Options.Misc.Screen.ShowAimbotFov, 235.0f);
			Custom::Checkbox3(XorStr("No Recoil"), &g_Options.Misc.Exploits.LocalPlayer.norecoil, 235.0f);
			Custom::SliderInt(XorStr("Max Distance"), &g_Options.LegitBot.AimBot.MaxDistance, 0, 100, XorStr("%dm"), 0, XorStr("Maximum range"));
		}
		ImGui::EndCustomChild();

		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2 + 10, 60));
		ImGui::CustomChild(XorStr("Settings Aim"), ImVec2(ImGui::GetWindowSize().x / 2 - 30, 350));
		{
			if (g_Options.LegitBot.AimBot.Silent || g_Options.LegitBot.AimBot.Enabled) {
				Custom::Checkbox3(XorStr("Agresive Mode"), &g_Options.LegitBot.AimBot.aggressiveMode, 235.0f);
				ImGui::KeyBind(XorStr("Key Silent Aim"), &g_Options.LegitBot.AimBot.KeyBind);
			}

			if (g_Options.Misc.Screen.ShowAimbotFov || g_Options.LegitBot.AimBot.Enabled)
			{
				Custom::SliderInt(XorStr("Field of view"), &g_Options.LegitBot.AimBot.FOV, 0, 600, XorStr("%d"), 0, XorStr("Adjust aimbot radius"));
				ImGui::ColorEdit4(XorStr("Fov Color"), g_Options.Misc.Screen.AimbotFovColor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
			}
		}
		ImGui::EndCustomChild();
	}

	void Interface::VisualsTab()
	{
		ImGui::SetCursorPos(ImVec2(20, 10));
		ImGui::SetWindowFontScale(1.2f);
		ImGui::Text(XorStr("Visuals"));
		ImGui::SetWindowFontScale(0.9f);

		ImGui::SetCursorPos(ImVec2(20, 26));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::Text(XorStr("Here are the visual options"));
		ImGui::PopFont();
		ImGui::PopStyleColor();
		ImGui::SetWindowFontScale(1.0f);

		ImGui::SetCursorPos(ImVec2(20, 60));
		ImGui::CustomChild(XorStr("General"), ImVec2(ImGui::GetWindowSize().x / 2 - 30, 340));
		{

			// Tu código existente en la parte del menú de ESP de jugadores:
			Custom::Checkbox3(XorStr("Enabled"), &g_Options.Visuals.ESP.Players.Enabled, 235.0f);
			Custom::Checkbox3(XorStr("Esp Box"), &g_Options.Visuals.ESP.Players.Box, 235.0f);
			/*if (g_Options.Visuals.ESP.Players.Box)
			{
				ImGui::Combo(XorStr("Box Style"), &g_Options.Visuals.ESP.Players.players_box, XorStr("None\0Full\0Cornered\0"));
			}*/
			Custom::Checkbox3(XorStr("Esp Health"), &g_Options.Visuals.ESP.Players.HealthBar, 235.0f);
			/*if (g_Options.Visuals.ESP.Players.HealthBar)
			{
				ImGui::Combo(XorStr("Healthbar"), &g_Options.Visuals.ESP.Players.players_healthbar, XorStr("None\0Right\0Left\0top\0Bottom\0"));
			}*/

			//Custom::Checkbox3(XorStr("Esp Skeleton"), &g_Options.Visuals.ESP.Players.Skeleton, 235.0f);
			Custom::Checkbox3(XorStr("Esp Username"), &g_Options.Visuals.ESP.Players.Name, 235.0f);
			Custom::Checkbox3(XorStr("Esp Distance"), &g_Options.Visuals.ESP.Players.Distance, 235.0f);
			Custom::Checkbox3(XorStr("Esp Line"), &g_Options.Visuals.ESP.Players.SnapLines, 235.0f);

			// ¡NUEVO! Opción para la Brújula ESP
			Custom::Checkbox3(XorStr("Esp Compass"), &g_Options.Visuals.ESP.Players.Compass.Enabled, 235.0f);

			// Opcional: Si quieres añadir controles para el radio, tamaño de punto y colores de la brújula:
			if (g_Options.Visuals.ESP.Players.Compass.Enabled)
			{
				ImGui::SliderFloat(XorStr("Compass Radius"), &g_Options.Visuals.ESP.Players.Compass.Radius, 50.0f, 200.0f, "%.1f");
				ImGui::SliderFloat(XorStr("Compass Dot Size"), &g_Options.Visuals.ESP.Players.Compass.DotSize, 1.0f, 10.0f, "%.1f");
				ImGui::ColorEdit4(XorStr("Compass Front Color"), g_Options.Visuals.ESP.Players.Compass.FrontColor);
				ImGui::ColorEdit4(XorStr("Compass Back Color"), g_Options.Visuals.ESP.Players.Compass.BackColor);
				ImGui::ColorEdit4(XorStr("Compass Circle Color"), g_Options.Visuals.ESP.Players.Compass.CircleColor);
			}
		}
		ImGui::EndCustomChild();

		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2 + 10, 60));
		ImGui::CustomChild(XorStr("Options"), ImVec2(ImGui::GetWindowSize().x / 2 - 30, 180));
		{
			Custom::SliderInt(XorStr("Render Distance"), &g_Options.Visuals.ESP.Players.RenderDistance, 0, 120, XorStr("%dm"), 0, XorStr("Adjust Render Distance"));
		}
		ImGui::EndCustomChild();

		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2 + 10, 260));
		ImGui::CustomChild(XorStr("Colors"), ImVec2(ImGui::GetWindowSize().x / 2 - 30, 200));
		{
			ImGui::ColorEdit4(XorStr("Skeleton Color"), g_Options.Visuals.ESP.Players.SkeletonColor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
			ImGui::ColorEdit4(XorStr("Name Color"), g_Options.Visuals.ESP.Players.NameColor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
			ImGui::ColorEdit4(XorStr("Distance Color"), g_Options.Visuals.ESP.Players.DistanceColor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
			ImGui::ColorEdit4(XorStr("SnapLines Color"), g_Options.Visuals.ESP.Players.SnapLinesColor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
			ImGui::ColorEdit4(XorStr("BoxColor Color"), g_Options.Visuals.ESP.Players.BoxColor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
		}
		ImGui::EndCustomChild();
	}

	void Interface::SettingsTab() {
		ImGui::SetCursorPos(ImVec2(20, 10));
		ImGui::SetWindowFontScale(1.2f);
		ImGui::Text(XorStr("Settings"));
		ImGui::SetWindowFontScale(0.9f);

		ImGui::SetCursorPos(ImVec2(20, 26));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::Text(XorStr("Settings menu options"));
		ImGui::PopFont();
		ImGui::PopStyleColor();
		ImGui::SetWindowFontScale(1.0f);

		ImGui::SetCursorPos(ImVec2(20, 60));
		ImGui::CustomChild(XorStr("Config System"), ImVec2(ImGui::GetWindowSize().x / 2 - 30, 200));
		{
			Custom::Checkbox3(XorStr("Stream Mode"), &g_Options.General.CaptureBypass, 235.0f);

			Custom::SliderInt(XorStr("Update Delay"), &g_Options.General.ThreadDelay, 0, 100, XorStr("%dms"), 0, XorStr("Adjust Update Delay"));

			ImGui::KeyBind(XorStr("Menu Bind"), &g_Options.General.MenuKey);

			if (ImGui::Button(XorStr("Close Menu")))
			{
				g_Options.General.ShutDown = true;
			}

			if (ImGui::Button(XorStr("Set High Priority"))) {
				HANDLE hProcess = GetCurrentProcess();
				SetProcessPriority(hProcess, HIGH_PRIORITY_CLASS);
			}
			if (ImGui::Button(XorStr("Set Normal Priority"))) {
				HANDLE hProcess = GetCurrentProcess();
				SetProcessPriority(hProcess, NORMAL_PRIORITY_CLASS);
			}
		}
		ImGui::EndCustomChild();
	}

	void Interface::RenderGui() {
		if (!bIsMenuOpen) {
			return;
		}

		float windowWidth = 600.0f;  
		float windowHeight = 450.0f; 

		ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
		ImGui::Begin(XorStr("M$n#"), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		{
			ImDrawList* DrawList = ImGui::GetWindowDrawList();
			ImVec2 Pos = ImGui::GetWindowPos();
			ImVec2 Size = ImGui::GetWindowSize();

		
			DrawList->AddRectFilled(Pos, Pos + ImVec2(80, Size.y), ImColor(15, 15, 16), ImGui::GetStyle().WindowRounding, ImDrawFlags_RoundCornersLeft);
			DrawList->AddRectFilled(Pos, Pos + ImVec2(Size.x, 50), ImColor(15, 15, 16), ImGui::GetStyle().WindowRounding, ImDrawFlags_RoundCornersTop);
			DrawList->AddLine(Pos + ImVec2(80, 0), Pos + ImVec2(80, Size.y), ImGui::GetColorU32(ImGuiCol_Border));
			DrawList->AddLine(Pos + ImVec2(80, 50), Pos + ImVec2(670, 50), ImGui::GetColorU32(ImGuiCol_Border));

			ImGui::BeginChild(XorStr("LeftChild"), ImVec2(80, Size.y));
			{
				ImGui::SetCursorPos(ImVec2(0, 20));
				ImGui::BeginChild(XorStr("Tabs"));
				Custom::Tab(ICON_FA_MOUSE, &CurrentTab, 1);  
				Custom::Tab(ICON_FA_EYE, &CurrentTab, 2);
				Custom::Tab(ICON_FA_FILE, &CurrentTab, 3);
				ImGui::EndChild();
			}
			ImGui::EndChild();

			static float AimBotChildAnim = 0;
			static float VisualsChildAnim = 0;
			static float SettingsChildAnim = 0;

			AimBotChildAnim = ImLerp(AimBotChildAnim, CurrentTab == 1 ? 0.f : Size.y, ImGui::GetIO().DeltaTime * 10.f);
			VisualsChildAnim = ImLerp(VisualsChildAnim, CurrentTab == 2 ? 0.f : Size.y, ImGui::GetIO().DeltaTime * 10.f);
			SettingsChildAnim = ImLerp(SettingsChildAnim, CurrentTab == 3 ? 0.f : Size.y, ImGui::GetIO().DeltaTime * 10.f);

			
			ImGui::SetCursorPos(ImVec2(80, AimBotChildAnim));
			ImGui::BeginChild(XorStr("AimBotMainChild"), ImVec2(Size.x - 80, Size.y));
			{
				AimbotTab();
			}
			ImGui::EndChild();

			ImGui::SetCursorPos(ImVec2(80, VisualsChildAnim));
			ImGui::BeginChild(XorStr("VisualsMainChild"), ImVec2(Size.x - 80, Size.y));
			{
				VisualsTab();
			}
			ImGui::EndChild();

			ImGui::SetCursorPos(ImVec2(80, SettingsChildAnim));
			ImGui::BeginChild(XorStr("SettingsMainChild"), ImVec2(Size.x - 80, Size.y));
			{
				SettingsTab();
			}
			ImGui::EndChild();
		}
		ImGui::End();
	}


	void Interface::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

		switch (uMsg) {
		case WM_SIZE:
			if (wParam != SIZE_MINIMIZED) {
				ResizeWidht = (UINT)LOWORD(lParam);
				ResizeHeight = (UINT)HIWORD(lParam);
			}
			break;
		}

		if (bIsMenuOpen) {
			ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		}
	}

	void Interface::HandleMenuKey()
	{
		static bool MenuKeyDown = false;
		if (GetAsyncKeyState(g_Options.General.MenuKey) & 0x8000)
		{
			if (!MenuKeyDown)
			{
				MenuKeyDown = true;
				bIsMenuOpen = !bIsMenuOpen;

				if (bIsMenuOpen) {
					SetWindowLong(hWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TOOLWINDOW /*| WS_EX_TRANSPARENT*/ );
					SetForegroundWindow(hWindow);
				} else {
					SetWindowLong(hWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_NOACTIVATE);
					SetForegroundWindow(hTargetWindow);
				}
				SetWindowPos(hWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
			}
		} else {
			MenuKeyDown = false;
		}
	}

	void Interface::ShutDown() {
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		FrameWork::Overlay::ShutDown();
	}
}
