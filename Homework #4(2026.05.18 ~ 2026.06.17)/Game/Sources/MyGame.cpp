#include "Precompiled.hpp"
#include "MyGame.hpp"

#include "Framework/Platform/WindowOptions.hpp"
#include "Framework/Platform/WindowSubsystem.hpp"

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
		WindowOptions options{};
		options.caption = TEXT("Homework #4(2026.05.18 ~ 2026.06.17)");
		options.position = Framework::Vector2D(100, 100);
		options.size = Framework::Vector2D(1280, 960);
		options.style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

		WindowSubsystem* windowSubsystem = MyEngine::GetInstance()->GetSubsystem<WindowSubsystem>();
		assert(windowSubsystem);

		auto created = windowSubsystem->Create(options);
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
