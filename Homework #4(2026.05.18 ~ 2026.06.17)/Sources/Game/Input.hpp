#pragma once

#include "Framework/Platform.hpp"

namespace TUK::Game
{
	namespace Input
	{
		void Startup(Framework::InputProvider& inputProvider);
		void Shutdown();

		[[nodiscard]] bool IsKeyPressed(int key);
		[[nodiscard]] bool IsKeyHeld(int key);
		[[nodiscard]] bool IsKeyReleased(int key);
	}
}