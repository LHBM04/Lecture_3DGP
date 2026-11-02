#pragma once

#include "../Framework/Core/Engine.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	class MyEngine : public Engine
	{
		using Engine::Engine;

	public:
		SystemStatus Startup() override;
	};
}
