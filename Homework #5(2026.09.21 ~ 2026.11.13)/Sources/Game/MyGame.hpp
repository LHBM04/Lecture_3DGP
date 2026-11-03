#pragma once

#include "../Framework/Core/App.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	class MyGame : public App
	{
		using App::App;

	public:
		SystemStatus Startup() override;
	};
}
