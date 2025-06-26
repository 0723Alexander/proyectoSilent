// Free-Fire-Internal/Cheat/Options.hpp
#pragma once
#include <Windows.h> 

namespace Cheat {
	class Options {
	public:
		struct LegitBot
		{
			struct AimBot
			{
				bool Enabled;
				bool Silent;
				bool aggressiveMode;
				int KeyBind = 0;
				int SilentKeyBin = 0;
				bool TargetNPC;
				bool VisibleCheck;
				int HitBox = 0;
				int MaxDistance = 50;
				int FOV = 50;
				bool IgnoreKnocked = false;
			} AimBot;
		} LegitBot;

		struct Visuals
		{
			struct ESP
			{
				struct Players
				{
					int KeyBind = 0;
					bool Enabled = true;
					bool ShowLocalPlayer = false;
					bool ShowNPCs;
					bool VisibleOnly = false;
					int RenderDistance = 60;
					//
					bool Box;
					int players_box = 1;
					float BoxColor[4] = { 1.f, 1.f, 1.f, 1.f };
					int boxRouding = 0;
					//
					bool HealthBar = false;
					int players_healthbar = 1;
					///
					bool Head = false;
					float HeadColor[4] = { 1.f, 1.f, 1.f, 1.f };
					///
					bool Skeleton = false;
					float SkeletonColor[4] = { 1.f, 1.f, 1.f, 1.f };
					///
					int players_nametag = 0;
					bool Name = false;
					float NameColor[4] = { 1.f, 1.f, 1.f, 1.f };
					///
					int players_armorbar = 0;
					bool ArmorBar;
					///
					bool Distance;
					float DistanceColor[4] = { 1.f, 1.f, 1.f, 1.f };
					///
					bool SnapLines;
					int EspLines = 0;
					float SnapLinesColor[4] = { 1.f, 1.f, 1.f, 1.f };

					bool ESPAimLine;
					float ESPAimLineColor[4] = { 1.f, 0.f, 0.f, 1.f };

					bool IgnoreVisibility;      // Para activar/desactivar la función
					int IgnoreVisibilityKey = 0; // Para guardar la tecla que lo activa (0 = ninguna)

					bool TargetLine;
					bool AimbotTargetLine;

					// NUEVAS OPCIONES PARA BRÚJULA ESP
					struct Compass
					{
						bool Enabled = false;
						float Radius = 100.0f; // Radio del círculo de la brújula
						float DotSize = 5.0f; // Tamaño del indicador (punto o triángulo)
						float FrontColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f }; // Verde para enemigos al frente
						float BackColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };  // Rojo para enemigos atrás
						float CircleColor[4] = { 0.5f, 0.5f, 0.5f, 0.5f }; // Color del círculo de la brújula
					} Compass;

				} Players;
			} ESP;
		} Visuals;

		struct Misc
		{
			struct Screen
			{
				bool ShowAimbotFov;
				float AimbotFovColor[4] = { 1.f, 1.f, 1.f, 1.f };
			} Screen;

			struct Exploits
			{
				struct LocalPlayer
				{
					bool norecoil;
					int health_ammount;
					bool Start_Health;
				} LocalPlayer;

			} Exploits;
		} Misc;

		struct General
		{
			char Username[25] = "";
			char PassWord[20] = "";
			bool ShutDown = false;
			int MenuKey = VK_INSERT;
			bool CaptureBypass = false;
			int ThreadDelay = 1;
		} General;
	};
}
inline Cheat::Options g_Options;