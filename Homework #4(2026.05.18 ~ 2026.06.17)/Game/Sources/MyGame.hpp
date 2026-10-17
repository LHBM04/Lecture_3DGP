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
		SystemStatus Ready() override;

	private:
		MyGame() noexcept;
		~MyGame() noexcept override;

		void RequireWindow();
		void RequireScenes();

		Framework::Display* window = nullptr;
	};
}
