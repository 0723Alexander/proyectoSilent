#ifndef FREEFIRESDK_HPP
#define FREEFIRESDK_HPP

#pragma once
#include <Cheat/FreeFireSDK/Context.hpp>
#include <FrameWork/FrameWork.hpp>

namespace Cheat {
	class FreeFireSDK {
	public:
		bool Setup();
	};
	inline FreeFireSDK g_FreeFire;
}

#endif