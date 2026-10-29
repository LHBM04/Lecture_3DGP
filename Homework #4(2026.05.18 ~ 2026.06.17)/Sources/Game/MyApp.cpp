#include "Precompiled.hpp"
#include "MyApp.hpp"

#include "Framework/Entities/WorldSubsystem.hpp"
#include "Framework/Graphics/RenderSubsystem.hpp"
#include "Framework/Platform/DisplayOptions.hpp"
#include "Framework/Platform/DisplaySubsystem.hpp"

#include "MyEngine.hpp"
#include "TestWorld.hpp"

DEFINE_APP_INSTANCE(::TUK::Game::MyApp)

namespace TUK::Game
{
	MyApp::MyApp() noexcept
		: window(nullptr)
	{
	}

	MyApp::~MyApp() noexcept
	{
		if (window)
		{
			window->SetShouldClose(true);
			window = nullptr;
		}
	}

	SystemStatus MyApp::Startup()
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

		WorldSubsystem* worldSubsystem = AddSubsystem<WorldSubsystem>();
		worldSubsystem->AddWorld(std::make_unique<TestWorld>());

		RenderSubsystem* renderSubsystem = MyEngine::GetInstance()->GetSubsystem<RenderSubsystem>();
		assert(renderSubsystem);
		renderSubsystem->Subscribe(worldSubsystem, &WorldSubsystem::PreRender);
		renderSubsystem->Subscribe(worldSubsystem, &WorldSubsystem::Render);
		renderSubsystem->Subscribe(worldSubsystem, &WorldSubsystem::PostRender);

		return App::Startup();
	}

	void MyApp::Shutdown()
	{
		// DisplaySubsystem이 닫힌 창을 먼저 해제할 수 있으므로 종료 후 창 포인터를 사용하지 않습니다.
		window = nullptr;
		App::Shutdown();
	}
}
