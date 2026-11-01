#pragma once

#include "../Framework/Core/Engine.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	class MyEngine : public Engine
	{
	public:
		using Engine::Engine;
		//using Engine::~Engine;

		SystemStatus Startup() override;
	};
}
