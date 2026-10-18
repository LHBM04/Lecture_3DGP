#pragma once

#include "Framework/Core/Game.hpp"

#include "Framework/Platform/Display.hpp"

using SystemStatus = TUK::Framework::SystemStatus;

namespace TUK::Game
{
	class MyGame : public Framework::Game
	{
		DECLARE_GAME_INSTANCE();

	public:
		MyGame() noexcept;
		~MyGame() noexcept override;

		SystemStatus Startup() override;

	private:
		Framework::Display* window;
	};
}
