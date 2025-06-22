#ifndef ADB_UTILS_HPP
#define ADB_UTILS_HPP

#include <FrameWork/FrameWork.hpp>
#include <FrameWork/Utilities/Android Debug Bridge (adb)/Adb.hpp>

namespace FrameWork {
	namespace ADB {
		std::string GetProcessPath(const std::wstring& processName);
		std::vector<std::string> GetAvailableDrives();
		std::string FindAdbPath(const std::string& emulatorFolder);
		std::string GetAdbPath();
		bool InitializeADB();
	};
}
#endif