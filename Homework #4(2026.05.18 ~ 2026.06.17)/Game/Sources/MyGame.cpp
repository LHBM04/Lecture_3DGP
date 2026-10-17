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
	{
	}

	MyGame::~MyGame() noexcept
	{
	}

	SystemStatus MyGame::Ready()
	{
		Game::Ready();

		DisplayOptions options{};
		options.caption = TEXT("Homework #4(2026.05.18 ~ 2026.06.17)");
		options.position = Framework::Vector2D(100, 100);
		options.size = Framework::Vector2D(1280, 960);

		DisplaySubsystem* windowSubsystem = MyEngine::GetInstance()->GetSubsystem<DisplaySubsystem>();
		assert(windowSubsystem);

		auto created = windowSubsystem->CreateDisplay(options);
		if (!created)
		{
			return SystemStatus::Failed;
		}
		window = &(created.value().get());

		return SystemStatus::Continue;
	}
}
