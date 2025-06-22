#pragma once

#include <FrameWork/FrameWork.hpp>
#include <d3d11.h>
#include <d3dx11.h>
extern std::string logMessage;

namespace FrameWork
{
	class Interface
	{
	public:
		Interface(HWND Window, HWND TargetWindow, ID3D11Device* Device, ID3D11DeviceContext* DeviceContext) { 
			Initialize(Window, TargetWindow, Device, DeviceContext); 
		}
		~Interface() { 
			ShutDown(); 
		}
		/// <Teste>
		static std::string GetLogMessage();
		static void SetLogMessage(const std::string& message);
		///
		void Initialize(HWND Window, HWND TargetWindow, ID3D11Device* Device, ID3D11DeviceContext* DeviceContext);
		void InitializeMenu();
		void UpdateStyle();

		void RenderGui();

		void WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		void HandleMenuKey();

		void ShutDown();

		bool GetMenuOpen() const { return bIsMenuOpen; }
	private:
		HWND hWindow;
		HWND hTargetWindow;
		ID3D11Device* IDevice;

		int CurrentTab = 0;
	
		bool bIsMenuOpen = false;

	private:
		void AimbotTab();

		void VisualsTab();

		void SettingsTab();

	public:
		UINT ResizeWidht;
		UINT ResizeHeight;
	};
}