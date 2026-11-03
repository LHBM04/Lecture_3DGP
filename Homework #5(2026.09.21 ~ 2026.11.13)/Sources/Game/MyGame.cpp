#include "Precompiled.hpp"
#include "MyGame.hpp"

#include "../Framework/Core.hpp"
#include "../Framework/Platform.hpp"
#include "../Framework/Entities/WorldSubsystem.hpp"
#include "../Framework/Graphics.hpp"

#include "Worlds/World_Stage.hpp"

#include "Resource.h"

namespace TUK::Game
{
	SystemStatus MyGame::Startup()
	{
		EventSubsystem* eventSubsystem = Engine::GetInstance().GetSubsystem<EventSubsystem>();
		if (!eventSubsystem)
		{
			return SystemStatus::FAILURE;
		}

		InputSubsystem* inputSubsystem = AddSubsystem<InputSubsystem>();
		if (const SystemStatus status = App::Startup(); status != SystemStatus::CONTINUE)
		{
			return status;
		}

		eventSubsystem->AddListener<InputActivationEvent>(inputSubsystem, &InputSubsystem::OnActivation);
		eventSubsystem->AddListener<DisplayCreateEvent>(inputSubsystem, &InputSubsystem::OnCreate);
		eventSubsystem->AddListener<DisplayCloseEvent>(inputSubsystem, &InputSubsystem::OnClose);
		eventSubsystem->AddListener<DisplayFocusEvent>(inputSubsystem, &InputSubsystem::OnFocus);

		DisplaySubsystem* displaySubsystem = Engine::GetInstance().GetSubsystem<DisplaySubsystem>();
		if (!displaySubsystem)
		{
			return SystemStatus::FAILURE;
		}

		TCHAR caption[256]{};
		LoadString(GetModuleHandle(nullptr), IDS_WNDCAPTION, caption, 256);

		DisplayOptions options{};
		options.caption = caption;
		options.position = Vector2D(100, 100);
		options.size = Vector2D(1280, 960);
		options.style = WS_OVERLAPPEDWINDOW;
		options.styleEx = WS_EX_APPWINDOW;
		options.show = SW_SHOW;

		auto result = displaySubsystem->CreateDisplay(options);
		if (!result)
		{
			return SystemStatus::FAILURE;
		}

		WorldSubsystem* worldSubsystem = AddSubsystem<WorldSubsystem>();
		worldSubsystem->AddWorld(std::make_unique<World_Stage>());

		RenderSubsystem* renderSubsystem = Engine::GetInstance().GetSubsystem<RenderSubsystem>();
		if (!renderSubsystem)
		{
			return SystemStatus::FAILURE;
		}

		renderSubsystem->AddListener(worldSubsystem, &WorldSubsystem::PreRender);
		renderSubsystem->AddListener(worldSubsystem, &WorldSubsystem::Render);
		renderSubsystem->AddListener(worldSubsystem, &WorldSubsystem::PostRender);

		return App::Startup();
	}
}
