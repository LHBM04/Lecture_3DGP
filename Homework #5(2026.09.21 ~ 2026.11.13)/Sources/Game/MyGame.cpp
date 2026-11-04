#include "Precompiled.hpp"
#include "MyGame.hpp"

#include "Worlds/World_Stage.hpp"

#include "Resource.h"

namespace TUK::Game
{
	SystemStatus MyGame::Startup()
	{
		displaySubsystem = Engine::GetInstance().GetSubsystem<DisplaySubsystem>();
		if (!displaySubsystem)
		{
			return SystemStatus::FAILURE;
		}
		
		renderSubsystem = Engine::GetInstance().GetSubsystem<RenderSubsystem>();
		if (!renderSubsystem)
		{
			return SystemStatus::FAILURE;
		}
		
		worldSubsystem = AddSubsystem<WorldSubsystem>();
		if (!worldSubsystem)
		{
			return SystemStatus::FAILURE;
		}

		inputSubsystem = AddSubsystem<InputSubsystem>();
		if (!inputSubsystem)
		{
			return SystemStatus::FAILURE;
		}

		assetSubsystem = AddSubsystem<AssetSubsystem>();
		if (!assetSubsystem)
		{
			return SystemStatus::FAILURE;
		}

		renderSubsystem->AddListener(worldSubsystem, &WorldSubsystem::PreRender);
		renderSubsystem->AddListener(worldSubsystem, &WorldSubsystem::Render);
		renderSubsystem->AddListener(worldSubsystem, &WorldSubsystem::PostRender);

		worldSubsystem->AddWorld(std::make_unique<World_Stage>());

		EventSubsystem* eventSubsystem = Engine::GetInstance().GetSubsystem<EventSubsystem>();
		if (!eventSubsystem)
		{
			return SystemStatus::FAILURE;
		}

		eventSubsystem->AddListener<InputActivationEvent>(inputSubsystem, &InputSubsystem::OnActivation);
		eventSubsystem->AddListener<DisplayCreateEvent>(inputSubsystem, &InputSubsystem::OnCreate);
		eventSubsystem->AddListener<DisplayCloseEvent>(inputSubsystem, &InputSubsystem::OnClose);
		eventSubsystem->AddListener<DisplayFocusEvent>(inputSubsystem, &InputSubsystem::OnFocus);

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

		auto& device = renderSubsystem->GetDevice();
		assetSubsystem->AddLoader<ModelAssetLoader>(device, *assetSubsystem);
		assetSubsystem->AddLoader<MaterialAssetLoader>(
			device,
			*assetSubsystem,
			DXGI_FORMAT_R8G8B8A8_UNORM
		);
		assetSubsystem->AddLoader<ShaderAssetLoader>();

		return App::Startup();
	}
}
