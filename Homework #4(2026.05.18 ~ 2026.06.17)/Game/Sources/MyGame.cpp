#include "Precompiled.hpp"
#include "MyGame.hpp"

#include "Framework/Platform/DisplayOptions.hpp"
#include "Framework/Platform/DisplaySubsystem.hpp"

#include "Framework/Graphics/RenderSubsystem.hpp"

#include "MyEngine.hpp"
#include "TestWorld.hpp"

DEFINE_GAME_INSTANCE(::TUK::Game::MyGame)

namespace TUK::Game
{
	MyGame::MyGame() noexcept
		: window(nullptr)
	{
	}

	MyGame::~MyGame() noexcept
	{
		if (window)
		{
			window->SetShouldClose(true);
			window = nullptr;
		}
	}

	SystemStatus MyGame::Startup()
	{
		DisplayOptions options{};
		options.caption = TEXT("Homework #4(2026.05.18 ~ 2026.06.17)");
		options.position = Framework::Vector2D(100, 100);
		options.size = Framework::Vector2D(1280, 960);

		DisplaySubsystem* windowSubsystem = MyEngine::GetInstance()->GetSubsystem<DisplaySubsystem>();
		assert(windowSubsystem);

		auto created = windowSubsystem->CreateDisplay(options);
		if (!created)
		{
			return SystemStatus::Failure;
		}
		window = &(created.value().get());

		return Game::Startup();
	}
}
