#pragma once

#include "Framework/Core/Game.hpp"

#include "Framework/Platform/Display.hpp"

namespace TUK::Game
{
	class MyGame : public Framework::Game
	{
		DECLARE_GAME_INSTANCE();

	public:
		void Ready() override;

	private:
		MyGame() noexcept;
		~MyGame() noexcept override;

		void RequireWindow();
		void RequireScenes();

		Framework::Display* window = nullptr;
	};
}
