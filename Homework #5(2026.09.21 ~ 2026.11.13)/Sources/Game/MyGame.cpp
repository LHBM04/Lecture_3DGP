#include "Precompiled.hpp"
#include "MyGame.hpp"

#include "../Framework/Platform.hpp"

#include "MyEngine.hpp"
#include "Resource.h"

namespace TUK::Game
{
	SystemStatus MyGame::Startup()
	{
		EventSubsystem* eventSubsystem = MyEngine::GetInstance().GetSubsystem<EventSubsystem>();
		if (!eventSubsystem)
		{
			return SystemStatus::FAILURE;
		}

		InputSubsystem* inputSubsystem = AddSubsystem<InputSubsystem>();
		if (const SystemStatus status = App::Startup(); status != SystemStatus::CONTINUE)
		{
			return status;
		}

		eventSubsystem->AddListener<KeyInterectEvent>(inputSubsystem, &InputSubsystem::OnKey);
		eventSubsystem->AddListener<ButtonInterectEvent>(inputSubsystem, &InputSubsystem::OnButton);
		eventSubsystem->AddListener<MouseMoveEvent>(inputSubsystem, &InputSubsystem::OnMouseMove);
		eventSubsystem->AddListener<MouseWheelEvent>(inputSubsystem, &InputSubsystem::OnMouseWheel);
		eventSubsystem->AddListener<MouseRawInputEvent>(inputSubsystem, &InputSubsystem::OnMouseRawInput);
		eventSubsystem->AddListener<InputActivationEvent>(inputSubsystem, &InputSubsystem::OnActivation);
		eventSubsystem->AddListener<DisplayCreateEvent>(inputSubsystem, &InputSubsystem::OnCreate);
		eventSubsystem->AddListener<DisplayCloseEvent>(inputSubsystem, &InputSubsystem::OnClose);
		eventSubsystem->AddListener<DisplayFocusEvent>(inputSubsystem, &InputSubsystem::OnFocus);

		DisplaySubsystem* displaySubsystem = MyEngine::GetInstance().GetSubsystem<DisplaySubsystem>();
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

		if (auto result = displaySubsystem->CreateDisplay(options); !result)
		{
			return SystemStatus::FAILURE;
		}

		return SystemStatus::CONTINUE;
	}
}
