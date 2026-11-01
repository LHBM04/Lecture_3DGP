#pragma once

#include "../Framework/Core/App.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	class MyGame : public App
	{
	public:
		MyGame() noexcept = default;
		~MyGame() noexcept override = default;

		SystemStatus Startup() override;
	};
}
