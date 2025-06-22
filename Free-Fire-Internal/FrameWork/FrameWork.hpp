#pragma once
#include <crtdbg.h>

// Windows Libraries
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <thread>
#include <dwmapi.h>
#include <algorithm>
#include <TlHelp32.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <filesystem>
#include <psapi.h>
#pragma comment(lib, "Psapi.lib")

// Security
#include <FrameWork/Security/XorStr.hpp>
#include <FrameWork/Security/LazyImporter.hpp>

// Utilities
#include <FrameWork/Utilities/Misc/Misc.hpp>
#include <FrameWork/Utilities/Memory/Memory.hpp>
#include <FrameWork/Utilities/FindRender/Render.hpp>
#include <FrameWork/Utilities/Android Debug Bridge (adb)/adb_utils.hpp>

// Render
#include <FrameWork/Render/Overlay.hpp>
#include <FrameWork/Render/interface.hpp>
#include <FrameWork/Render/Assets.hpp>
#include <FrameWork/Render/Assets/FontAwesome.hpp>
#include <FrameWork/Render/Assets/FontInter.hpp>
#include <FrameWork/Render/Assets/FontAwesome.hpp>

// ImGui
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include <FrameWork/Dependencies/ImGui/Custom/Custom.h>

// DirectX Inlcude
#include <D3D11.h>
#include <D3DX11.h>

#define XorStr(str) jmXorStr(str)
#define SafeCall(str) lzimpLI_FN(str)