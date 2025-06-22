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
					bool norecoil = false;
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