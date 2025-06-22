#include <FrameWork/Render/Overlay.hpp>

struct WndRECT : public RECT {
	int Width() { return right - left; }
	int Height() { return bottom - top; }
};

static inline std::function<void(HWND, UINT, WPARAM, LPARAM)> pWindowProc;

bool bSettuped = false;
bool bInitialized = false;
bool bDeviceInitialized;
bool bRenderTargetInitialized;

HWND hWindow = nullptr;
WNDCLASSEX WindowClass;
HWND hTargetWindow = nullptr;
WndRECT wTargetWindowRect;

ID3D11Device* ID3dDevice;
ID3D11DeviceContext* ID3dDeviceContext;
IDXGISwapChain* ID3dSwapChain;
ID3D11RenderTargetView* ID3dRenderTargetView;

void CreateDeviceD3D() {

	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));
	SwapChainDesc.BufferDesc.Width = 0;
	SwapChainDesc.BufferDesc.Height = 0;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 75;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.BufferCount = 2;
	SwapChainDesc.OutputWindow = hWindow;
	SwapChainDesc.Windowed = TRUE;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D_FEATURE_LEVEL FeatureLevel;
	const D3D_FEATURE_LEVEL FeatureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, FeatureLevelArray, 2, D3D11_SDK_VERSION, &SwapChainDesc, &ID3dSwapChain, &ID3dDevice, &FeatureLevel, &ID3dDeviceContext) != S_OK) {
		FrameWork::Misc::Log(XorStr("[ERROR] FrameWork::Window::InitializeDirectX11::D3D11CreateDeviceAndSwapChain Error: "), SafeCall(GetLastError)());
		return;
	}
	bDeviceInitialized = true;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (pWindowProc)
		pWindowProc(hWnd, uMsg, wParam, lParam);

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

namespace FrameWork {

	void Overlay::Setup(HWND TargetHWND) {

		hTargetWindow = TargetHWND;
		if (hTargetWindow) {
			FrameWork::Misc::Log(XorStr("[SUCCESS] FrameWork::Window::Setup successfully."));
			if (!GetClientRect(hTargetWindow, &wTargetWindowRect)) {
				FrameWork::Misc::Log(XorStr("[ERROR] FrameWork::Window::Setup GetClientRect failed."));
				return;
			}
			if (!MapWindowPoints(hTargetWindow, nullptr, reinterpret_cast<LPPOINT>(&wTargetWindowRect), 2)) {
				FrameWork::Misc::Log(XorStr("[ERROR] FrameWork::Window::Setup MapWindowPoints failed."));
				return;
			}

			bSettuped = true;
		}
		else {
			FrameWork::Misc::Log(XorStr("[ERROR] FrameWork::Window::Setup Window Not Found!"));
		}
	}

	void Overlay::Initialize() {

		if (!bSettuped) {
			std::cout << XorStr("[ERROR : FrameWork::Window::Initialize] Overlay Not Settuped!") << std::endl;
			return;
		}

		srand(static_cast<unsigned int>(time(0)));
		WindowClass.lpszClassName = FrameWork::Misc::RandomString(10).c_str();

		FrameWork::Misc::Log("[INFO] Attempting to unregister existing window class...");

		if (WindowClass.lpszClassName != nullptr) {
			if (SafeCall(UnregisterClass)(WindowClass.lpszClassName, WindowClass.hInstance))
				FrameWork::Misc::Log("[INFO] Successfully unregistered existing window class.");
		}
		else {
			DWORD unregErrorCode = GetLastError();
			FrameWork::Misc::Log("[WARNING] Failed to unregister existing window class. Error code: ", unregErrorCode);
		}

		WindowClass.cbSize = sizeof(WindowClass);
		WindowClass.style = CS_HREDRAW | CS_VREDRAW;
		WindowClass.lpfnWndProc = WindowProc;
		WindowClass.cbClsExtra = 0;
		WindowClass.cbWndExtra = 0;
		WindowClass.hInstance = GetModuleHandle(NULL);
		WindowClass.hIcon = NULL;
		WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		WindowClass.hbrBackground = HBRUSH(RGB(0, 0, 0));
		WindowClass.lpszMenuName = NULL;
		WindowClass.hIconSm = NULL;

		ATOM Class = SafeCall(RegisterClassEx)(&WindowClass);

		if (!Class) {
			DWORD errorCode = GetLastError();
			FrameWork::Misc::Log("[ERROR] FrameWork::Window::Initialize::RegisterClassEx Error: ", errorCode);
			return;
		}

		hWindow = CreateWindowEx(
			WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
			WindowClass.lpszClassName,
			WindowClass.lpszMenuName,
			WS_POPUP | WS_VISIBLE,
			wTargetWindowRect.left,
			wTargetWindowRect.top,
			wTargetWindowRect.Width(),
			wTargetWindowRect.Height(),
			NULL, NULL,
			GetModuleHandle(NULL),
			NULL
		);

		if (!hWindow) {
			DWORD errorCode = GetLastError();
			FrameWork::Misc::Log("[ERROR] FrameWork::Window::Initialize::CreateWindowEx Error: ", errorCode);
			return;
		}

		MARGINS Margins = { wTargetWindowRect.left, wTargetWindowRect.top, wTargetWindowRect.Width(), wTargetWindowRect.Height() };
		DwmExtendFrameIntoClientArea(hWindow, &Margins);

		SafeCall(SetLayeredWindowAttributes)(hWindow, RGB(0, 0, 0), 255, LWA_ALPHA);

		SafeCall(ShowWindow)(hWindow, SW_SHOWDEFAULT);
		SafeCall(UpdateWindow)(hWindow);

		bInitialized = true;

		dxInitialize();
	}

	void Overlay::ShutDown() {
		SafeCall(dxShutDown)();
		if (hWindow) {
			SafeCall(DestroyWindow)(hWindow);
			hWindow = nullptr;
		}
		if (WindowClass.lpszClassName) {
			SafeCall(UnregisterClass)(WindowClass.lpszClassName, WindowClass.hInstance);
			WindowClass.lpszClassName = nullptr;
		}
		bInitialized = false;
		bSettuped = false;
	}

	void FrameWork::Overlay::UpdateWindowPos() {
		WndRECT TargetWindowRect;
		GetClientRect(hTargetWindow, &TargetWindowRect);
		MapWindowPoints(hTargetWindow, nullptr, reinterpret_cast<LPPOINT>(&TargetWindowRect), 2);
		MoveWindow(hWindow, TargetWindowRect.left, TargetWindowRect.top, TargetWindowRect.Width(), TargetWindowRect.Height(), false);
		Context::WindowWidth = TargetWindowRect.Width();
		Context::WindowHeight = TargetWindowRect.Height();
	}


	void Overlay::SetupWindowProcHook(std::function<void(HWND, UINT, WPARAM, LPARAM)> Funtion) {
		pWindowProc = Funtion;
	}

	void Overlay::dxInitialize() {
		CreateDeviceD3D();
		if (bDeviceInitialized) {
			dxCreateRenderTarget();
		}
	}

	void Overlay::dxRefresh() {
		ID3dDeviceContext->OMSetRenderTargets(1, &ID3dRenderTargetView, nullptr);
		static float TransparentColor[4] = { 0, 0, 0, 0 };
		ID3dDeviceContext->ClearRenderTargetView(ID3dRenderTargetView, TransparentColor);
	}

	void Overlay::dxShutDown() {
		dxCleanupRenderTarget();
		dxCleanupDeviceD3D();
	}

	void Overlay::dxCreateRenderTarget() {
		ID3D11Texture2D* pBackBuffer;
		ID3dSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		ID3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &ID3dRenderTargetView);
		pBackBuffer->Release();

		bRenderTargetInitialized = true;
	}

	void Overlay::dxCleanupRenderTarget() {

		if (ID3dRenderTargetView) {
			ID3dRenderTargetView->Release();
			ID3dRenderTargetView = NULL;
		}

		bRenderTargetInitialized = false;
	}

	void Overlay::dxCleanupDeviceD3D() {

		if (ID3dRenderTargetView) {
			ID3dRenderTargetView->Release();
			ID3dRenderTargetView = NULL;
		}
		if (ID3dSwapChain) {
			ID3dSwapChain->Release();
			ID3dSwapChain = NULL;
		}
		if (ID3dDeviceContext) {
			ID3dDeviceContext->Release();
			ID3dDeviceContext = NULL;
		}
		if (ID3dDevice) {
			ID3dDevice->Release();
			ID3dDevice = NULL;
		}
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		bDeviceInitialized = false;
	}

	bool Overlay::IsSettuped() { return bSettuped; }
	bool Overlay::IsInitialized() { return bInitialized; }
	HWND Overlay::GetOverlayWindow() { return hWindow; }
	HWND Overlay::GetTargetWindow() { return hTargetWindow; }

	ID3D11Device* Overlay::dxGetDevice() { return ID3dDevice; }
	ID3D11DeviceContext* Overlay::dxGetDeviceContext() { return ID3dDeviceContext; }
	IDXGISwapChain* Overlay::dxGetSwapChain() { return ID3dSwapChain; }
	ID3D11RenderTargetView* Overlay::dxGetRenderTarget() { return ID3dRenderTargetView; }
}