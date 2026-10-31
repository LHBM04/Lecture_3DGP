#pragma once

#include "../Framework/Core/App.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	class MyGame : public App
	{
	public:
		MyGame() noexcept;
		~MyGame() noexcept override;

		SystemStatus Startup() override;
	};

	inline MyGame::MyGame() noexcept
	{
	}

	inline MyGame::~MyGame() noexcept
	{
	}
}
