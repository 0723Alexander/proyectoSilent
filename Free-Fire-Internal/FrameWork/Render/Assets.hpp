#pragma once

#include <imgui.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <FrameWork/FrameWork.hpp>
#include <FrameWork/Render/Assets/FontInter.hpp>
#include <FrameWork/Render/Assets/FontAwesome.hpp>

namespace FrameWork {
	namespace Assets {

		inline ImFont* FontAwesomeRegular = nullptr;
		inline ImFont* FontAwesomeSolid = nullptr;
		inline ImFont* FontAwesomeSolid18 = nullptr;
		inline ImFont* FontAwesomeSolidBig = nullptr;
		inline ImFont* tab_font = nullptr;
		inline ImFont* InterExtraLight = nullptr;
		inline ImFont* InterLight = nullptr;
		inline ImFont* InterThin = nullptr;
		inline ImFont* InterRegular = nullptr;
		inline ImFont* InterRegular14 = nullptr;
		inline ImFont* InterMedium = nullptr;
		inline ImFont* InterSemiBold = nullptr;
		inline ImFont* InterBold = nullptr;
		inline ImFont* InterBold12 = nullptr;
		inline ImFont* InterExtraBold = nullptr;
		inline ImFont* InterBlack = nullptr;

		inline ID3D11ShaderResourceView* LogoMenu;
		inline ID3D11ShaderResourceView* LogoLogin;

		void Initialize(ID3D11Device* Device);
	}
}