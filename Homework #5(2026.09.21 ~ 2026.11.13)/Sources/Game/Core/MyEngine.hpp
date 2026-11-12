#pragma once

#include "Framework/Core.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Platform.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	class MyEngine : public Engine
	{
		using Engine::Engine;

	public:
		SystemStatus Startup() override;

	private:
		DisplaySubsystem* displaySubsystem = nullptr;
		RenderSubsystem* renderSubsystem = nullptr;
		EventSubsystem* eventSubsystem = nullptr;
	};
}
