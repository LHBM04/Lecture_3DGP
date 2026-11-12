#pragma once

#include "Framework/Core.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Platform.hpp"

namespace TUK::Game
{
	using namespace Framework;

	class MyEngine : public Engine
	{
	public:
		using Engine::Engine;

		SystemStatus Startup() override;

	private:
		void OnRenderSubsystemAdded(RenderSubsystem* subsystem);

		DisplaySubsystem* displaySubsystem = nullptr;
		RenderSubsystem* renderSubsystem = nullptr;
		EventSubsystem* eventSubsystem = nullptr;
	};
}
