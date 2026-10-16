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
	void MyGame::Ready()
	{
		Game::Ready();

		RequireWindow();
		if (GetQuitCode() != EXIT_SUCCESS)
		{
			return;
		}
		RequireScenes();
	}

	MyGame::MyGame() noexcept
	{
	}

	MyGame::~MyGame() noexcept
	{
	}

	void MyGame::RequireWindow()
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
			ReportError(created.error());
			return;
		}
		window = &(created.value().get());

		RenderSubsystem* renderSubsystem = MyEngine::GetInstance()->GetSubsystem<RenderSubsystem>();
		assert(renderSubsystem);

		// renderSubsystem->RegisterWindow(*window);
	}

	void MyGame::RequireScenes()
	{
		/*
		SceneSubsystem* sceneSubsystem = AddSubsystem<SceneSubsystem>();
		assert(sceneSubsystem);

		sceneSubsystem->AddScene<Scene_Stage1>(0, L"Stage1", *window);
		*/
	}
}
