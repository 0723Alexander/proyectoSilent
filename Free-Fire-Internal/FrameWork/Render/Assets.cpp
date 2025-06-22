#include <FrameWork/Render/Assets.hpp>
#include <FrameWork/Render/Assets/ImagesBytes.hpp>
#include <FrameWork/Dependencies/ImGui/Custom/TabFonts.hpp>

namespace FrameWork {
	void Assets::Initialize(ID3D11Device* Device) {

		ImGuiIO& io = ImGui::GetIO();

		InterBlack = io.Fonts->AddFontFromMemoryCompressedTTF(InterBlack_compressed_data, InterBlack_compressed_size, 14);
		InterBold = io.Fonts->AddFontFromMemoryCompressedTTF(InterBold_compressed_data, InterBold_compressed_size, 17);
		InterBold12 = io.Fonts->AddFontFromMemoryCompressedTTF(InterBold_compressed_data, InterBold_compressed_size, 15);
		InterExtraBold = io.Fonts->AddFontFromMemoryCompressedTTF(InterExtraBold_compressed_data, InterExtraBold_compressed_size, 13);
		InterExtraLight = io.Fonts->AddFontFromMemoryCompressedTTF(InterExtraLight_compressed_data, InterExtraLight_compressed_size, 14);
		InterLight = io.Fonts->AddFontFromMemoryCompressedTTF(InterLight_compressed_data, InterLight_compressed_size, 12);
		InterMedium = io.Fonts->AddFontFromMemoryCompressedTTF(InterMedium_compressed_data, InterMedium_compressed_size, 17);
		InterRegular = io.Fonts->AddFontFromMemoryCompressedTTF(InterRegular_compressed_data, InterRegular_compressed_size, 17);
		InterRegular14 = io.Fonts->AddFontFromMemoryCompressedTTF(InterRegular_compressed_data, InterRegular_compressed_size, 15);
		InterSemiBold = io.Fonts->AddFontFromMemoryCompressedTTF(InterSemiBold_compressed_data, InterSemiBold_compressed_size, 16);
		InterThin = io.Fonts->AddFontFromMemoryCompressedTTF(InterThin_compressed_data, InterThin_compressed_size, 14);

		static const ImWchar IconRanges[] =
		{
			ICON_MIN_FA, ICON_MAX_FA, 0
		};

		ImFontConfig FontAwesomeConfig;
		FontAwesomeConfig.GlyphMinAdvanceX = 25.f * (2.0f / 3.0f);

		FontAwesomeRegular = io.Fonts->AddFontFromMemoryCompressedTTF(FontAwesomeRegular_compressed_data, FontAwesomeRegular_compressed_size, 25.f * (2.0f / 3.0f), &FontAwesomeConfig, &IconRanges[0]);
		FontAwesomeSolid = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(FontAwesomeSolid_compressed_data, FontAwesomeSolid_compressed_size, 27.f * (2.0f / 3.0f), &FontAwesomeConfig, &IconRanges[0]);
		FontAwesomeSolid18 = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(FontAwesomeSolid_compressed_data, FontAwesomeSolid_compressed_size, 18.f * (2.0f / 3.0f), &FontAwesomeConfig, &IconRanges[0]);
		FontAwesomeSolidBig = io.Fonts->AddFontFromMemoryCompressedTTF(FontAwesomeSolid_compressed_data, FontAwesomeSolid_compressed_size, 30.f * (2.0f / 3.0f), &FontAwesomeConfig, &IconRanges[0]);
		tab_font = io.Fonts->AddFontFromMemoryTTF(icomoon, sizeof(icomoon), 30, &FontAwesomeConfig, io.Fonts->GetGlyphRangesJapanese());
		D3DX11CreateShaderResourceViewFromMemory(Device, LogoMenuRawBytes, sizeof(LogoMenuRawBytes), NULL, NULL, &LogoMenu, NULL);
		
	}
}