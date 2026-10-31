#include "Precompiled.hpp"
#include "MyGame.hpp"

#include "../Framework/Platform/DisplaySubsystem.hpp"
#include "../Framework/Platform/DisplayOptions.hpp"

#include "MyEngine.hpp"
#include "Resource.h"

namespace TUK::Game
{
	SystemStatus MyGame::Startup()
	{
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
