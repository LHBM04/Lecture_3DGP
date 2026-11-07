#include "Precompiled.hpp"
#include "MyGame.hpp"

#include "../Framework/Core.hpp"
#include "../Framework/Entities/WorldSubsystem.hpp"
#include "../Framework/Graphics.hpp"
#include "../Framework/Platform.hpp"
#include "../Framework/Utilities/Logger.hpp"

#include "Worlds/World_Stage.hpp"

namespace TUK::Game
{
	SystemStatus MyGame::Startup()
	{
		EventSubsystem* eventSubsystem = Engine::GetInstance().GetSubsystem<EventSubsystem>();
		if (!eventSubsystem)
		{
			Logger::LogError("Failed to get Event Subsystem.");
			return SystemStatus::FAILURE;
		}

		inputSubsystem = AddSubsystem<InputSubsystem>();
		if (!inputSubsystem)
		{
			Logger::LogError("Failed to get Input Subsystem.");
			return SystemStatus::FAILURE;
		}

		eventSubsystem->AddListener<InputActivationEvent>(inputSubsystem, &InputSubsystem::OnInputActivation);
		eventSubsystem->AddListener<DisplayCreateEvent>(inputSubsystem, &InputSubsystem::OnDisplayCreate);
		eventSubsystem->AddListener<DisplayCloseEvent>(inputSubsystem, &InputSubsystem::OnDisplayClose);
		eventSubsystem->AddListener<DisplayFocusEvent>(inputSubsystem, &InputSubsystem::OnDisplayFocus);

		displaySubsystem = Engine::GetInstance().GetSubsystem<DisplaySubsystem>();
		if (!displaySubsystem)
		{
			Logger::LogError("Failed to get Display Subsystem.");
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
			Logger::LogError("Display creation failed: {}", result.error());
			return SystemStatus::FAILURE;
		}

		worldSubsystem = AddSubsystem<WorldSubsystem>();
		worldSubsystem->AddWorld(std::make_unique<World_Stage>());

		renderSubsystem = Engine::GetInstance().GetSubsystem<RenderSubsystem>();
		if (!renderSubsystem)
		{
			Logger::LogError("Failed to get Render Subsystem.");
			return SystemStatus::FAILURE;
		}

		renderSubsystem->AddListener(worldSubsystem, &WorldSubsystem::PreRender);
		renderSubsystem->AddListener(worldSubsystem, &WorldSubsystem::Render);
		renderSubsystem->AddListener(worldSubsystem, &WorldSubsystem::PostRender);

		assetSubsystem = AddSubsystem<AssetSubsystem>();
		if (!assetSubsystem)
		{
			Logger::LogError("Failed to get Asset Subsystem.");
			return SystemStatus::FAILURE;
		}

		auto& device = renderSubsystem->GetDevice();
		assetSubsystem->AddLoader<ModelAssetLoader>();
		assetSubsystem->AddLoader<MeshAssetLoader>(device);
		assetSubsystem->AddLoader<ImageAssetLoader>(device);
		assetSubsystem->AddLoader<MaterialAssetLoader>(assetSubsystem, device, DXGI_FORMAT_R8G8B8A8_UNORM);
		assetSubsystem->AddLoader<ShaderAssetLoader>();

		return App::Startup();
	}
}
