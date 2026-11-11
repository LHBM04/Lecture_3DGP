#include "Precompiled.hpp"
#include "MyGame.hpp"

#include <functional>
#include <utility>

#include "../Framework/Core.hpp"
#include "../Framework/Objects/WorldSubsystem.hpp"
#include "../Framework/Graphics.hpp"
#include "../Framework/Resources.hpp"
#include "../Framework/Platform.hpp"

#include "Worlds/World_Stage.hpp"
#include "Worlds/World_Title.hpp"

#include "Time.hpp"
#include "Input.hpp"

namespace TUK::Game
{
	SystemStatus MyGame::Startup()
	{
		EventSubsystem* eventSubsystem = Engine::GetInstance().GetSubsystem<EventSubsystem>();
		if (!eventSubsystem)
		{
			LOG_ERROR("이벤트 서브시스템을 가져오지 못했습니다.");
			return SystemStatus::FAILURE;
		}

		inputSubsystem = AddSubsystem<InputSubsystem>();
		if (!inputSubsystem)
		{
			LOG_ERROR("입력 서브시스템을 가져오지 못했습니다.");
			return SystemStatus::FAILURE;
		}

		eventSubsystem->OnEvent<InputActivationEvent>() += std::bind_front(&InputSubsystem::OnInputActivation, inputSubsystem);
		eventSubsystem->OnEvent<DisplayCreateEvent>() += std::bind_front(&InputSubsystem::OnDisplayCreate, inputSubsystem);
		eventSubsystem->OnEvent<DisplayCloseEvent>() += std::bind_front(&InputSubsystem::OnDisplayClose, inputSubsystem);
		eventSubsystem->OnEvent<DisplayFocusEvent>() += std::bind_front(&InputSubsystem::OnDisplayFocus, inputSubsystem);

		displaySubsystem = Engine::GetInstance().GetSubsystem<DisplaySubsystem>();
		if (!displaySubsystem)
		{
			LOG_ERROR("디스플레이 서브시스템을 가져오지 못했습니다.");
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
			LOG_ERROR("디스플레이 생성에 실패했습니다: {}", result.error());
			return SystemStatus::FAILURE;
		}

		worldSubsystem = AddSubsystem<WorldSubsystem>();
		auto stageWorld = std::make_unique<World_Stage>();
		worldSubsystem->AddWorld(std::make_unique<World_Title>(stageWorld.get(), &worldTransition));
		worldSubsystem->AddWorld(std::move(stageWorld));

		renderSubsystem = Engine::GetInstance().GetSubsystem<RenderSubsystem>();
		if (!renderSubsystem)
		{
			LOG_ERROR("렌더 서브시스템을 가져오지 못했습니다.");
			return SystemStatus::FAILURE;
		}

		renderSubsystem->OnRender() += std::bind_front(&WorldSubsystem::PreRender, worldSubsystem);
		renderSubsystem->OnRender() += std::bind_front(&WorldSubsystem::Render, worldSubsystem);
		renderSubsystem->OnRender() += std::bind_front(&WorldSubsystem::PostRender, worldSubsystem);

		assetSubsystem = AddSubsystem<AssetSubsystem>();
		if (!assetSubsystem)
		{
			LOG_ERROR("에셋 서브시스템을 가져오지 못했습니다.");
			return SystemStatus::FAILURE;
		}

		worldSubsystem->OnEnterWorld() += std::bind_front(&AssetSubsystem::OnWorldEntered, assetSubsystem);
		worldSubsystem->OnExitWorld() += std::bind_front(&AssetSubsystem::OnWorldExited, assetSubsystem);

		auto& device = renderSubsystem->GetDevice();
		assetSubsystem->AddLoader<ModelAssetLoader>();
		assetSubsystem->AddLoader<MeshAssetLoader>(device);
		assetSubsystem->AddLoader<TextureAssetLoader>(device);
		assetSubsystem->AddLoader<CubemapLoader>(device);
		assetSubsystem->AddLoader<MaterialAssetLoader>(device);
		assetSubsystem->AddLoader<ShaderAssetLoader>();
		assetSubsystem->AddLoader<FontAssetLoader>();
		assetSubsystem->AddLoader<AnimationAssetLoader>();

		eventSubsystem->OnEvent<DisplayCreateEvent>() += [this](const DisplayCreateEvent& event)
		{
			const InputProvider* provider = inputSubsystem->GetProvider(event.display);
			Input::Initialize(provider);
		};
		eventSubsystem->OnEvent<DisplayCloseEvent>() += [](const DisplayCloseEvent&)
		{
			Input::Release();
		};

		if (auto initialized = worldTransition.Initialize(worldSubsystem, inputSubsystem, device, assetSubsystem); !initialized)
		{
			LOG_ERROR("월드 전환 초기화에 실패했습니다: {}", initialized.error());
			return SystemStatus::FAILURE;
		}

		transitionRenderListenerId = renderSubsystem->OnRender().AddListener([this](GPUCommandBuffer& commandBuffer)
		{
			if (auto rendered = worldTransition.GetTransition().Render(commandBuffer); !rendered)
			{
				LOG_ERROR("페이드 렌더링에 실패했습니다: {}", rendered.error());
			}
		});

		Time::Initialize(Engine::GetInstance().GetSubsystem<TimeSubsystem>());
		return App::Startup();
	}

	SystemStatus MyGame::Routine()
	{
		worldTransition.Update(Time::GetUnscaledDeltaTime());
		return App::Routine();
	}

	void MyGame::Shutdown()
	{
		if (renderSubsystem)
		{
			renderSubsystem->OnRender().RemoveListener(transitionRenderListenerId);
			transitionRenderListenerId = 0;
		}
		worldTransition.Shutdown();

		App::Shutdown();
		Time::Release();
	}
}
