#pragma once

#include "Framework/Platform.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	/** 편하게 쓰기 위한 래퍼 */
	namespace Input
	{
		void Initialize(const InputProvider* newProvide);
		void Release();

		bool IsKeyPressed(KeyCode keyCode);
		bool IsKeyHeld(KeyCode keyCode);
		bool IsKeyReleased(KeyCode keyCode);

		bool IsMouseButtonPressed(ButtonCode button);
		bool IsMouseButtonHeld(ButtonCode button);
		bool IsMouseButtonReleased(ButtonCode button);
	}
}
