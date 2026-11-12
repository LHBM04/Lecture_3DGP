#include "Precompiled.hpp"
#include "MyGame.hpp"

#include "Framework/Core.hpp"
#include "Framework/GameObjects/SceneSubsystem.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Resources.hpp"
#include "Framework/Platform.hpp"

#include "../Scenes/Scene_Stage.hpp"
#include "../Scenes/Scene_Title.hpp"

#include "Assets.hpp"
#include "Time.hpp"
#include "Input.hpp"

namespace TUK::Game
{
	SystemStatus MyGame::Startup()
	{
		eventSubsystem = Engine::GetInstance().GetSubsystem<EventSubsystem>();
		if (!eventSubsystem)
		{
			LOG_ERROR("이벤트 서브시스템을 가져오지 못했습니다.");
			return SystemStatus::FAILURE;
		}

		displaySubsystem = Engine::GetInstance().GetSubsystem<DisplaySubsystem>();
		if (!displaySubsystem)
		{
			LOG_ERROR("디스플레이 서브시스템을 가져오지 못했습니다.");
			return SystemStatus::FAILURE;
		}

		renderSubsystem = Engine::GetInstance().GetSubsystem<RenderSubsystem>();
		if (!renderSubsystem)
		{
			LOG_ERROR("렌더 서브시스템을 가져오지 못했습니다.");
			return SystemStatus::FAILURE;
		}

		OnSubsystemAdded<InputSubsystem>(this, &MyGame::OnInputSubsystemAdded);
		OnSubsystemRemoved<InputSubsystem>(this, &MyGame::OnInputSubsystemRemoved);
		OnSubsystemAdded<SceneSubsystem>(this, &MyGame::OnSceneSubsystemAdded);
		OnSubsystemAdded<AssetSubsystem>(this, &MyGame::OnAssetSubsystemAdded);
		OnSubsystemRemoved<AssetSubsystem>(this, &MyGame::OnAssetSubsystemRemoved);

		inputSubsystem = AddSubsystem<InputSubsystem>();
		if (!inputSubsystem)
		{
			LOG_ERROR("입력 서브시스템을 가져오지 못했습니다.");
			return SystemStatus::FAILURE;
		}

		sceneSubsystem = AddSubsystem<SceneSubsystem>();
		if (!sceneSubsystem)
		{
			LOG_ERROR("월드 서브시스템을 가져오지 못했습니다.");
			return SystemStatus::FAILURE;
		}

		assetSubsystem = AddSubsystem<AssetSubsystem>();
		if (!assetSubsystem)
		{
			LOG_ERROR("에셋 서브시스템을 가져오지 못했습니다.");
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

		if (auto initialized = sceneTransition.Initialize(sceneSubsystem, inputSubsystem, renderSubsystem->GetDevice(), assetSubsystem); !initialized)
		{
			LOG_ERROR("월드 전환 초기화에 실패했습니다: {}", initialized.error());
			return SystemStatus::FAILURE;
		}

		transitionRenderListenerId = renderSubsystem->OnRender().AddListener([this](GPUCommandBuffer& commandBuffer)
		{
			if (auto rendered = sceneTransition.GetTransition().Render(commandBuffer); !rendered)
			{
				LOG_ERROR("페이드 렌더링에 실패했습니다: {}", rendered.error());
			}
		});

		return App::Startup();
	}

	SystemStatus MyGame::Routine()
	{
		sceneTransition.Update(Time::GetUnscaledDeltaTime());
		return App::Routine();
	}

	void MyGame::Shutdown()
	{
		if (renderSubsystem)
		{
			renderSubsystem->OnRender().RemoveListener(transitionRenderListenerId);
			transitionRenderListenerId = 0;
		}
		sceneTransition.Shutdown();

		App::Shutdown();
	}

	void MyGame::OnInputSubsystemAdded(InputSubsystem* subsystem)
	{
		ASSERT_FMT(eventSubsystem, "이벤트 서브시스템이 nullptr입니다.");

		eventSubsystem->Subscribe<InputActivationEvent>(subsystem, &InputSubsystem::OnInputActivation);
		eventSubsystem->Subscribe<DisplayCreateEvent>(subsystem, &InputSubsystem::OnDisplayCreate);
		eventSubsystem->Subscribe<DisplayCloseEvent>(subsystem, &InputSubsystem::OnDisplayClose);
		eventSubsystem->Subscribe<DisplayFocusEvent>(subsystem, &InputSubsystem::OnDisplayFocus);

		eventSubsystem->Subscribe<DisplayCreateEvent>([subsystem](const DisplayCreateEvent& event)
		{
			const InputProvider* provider = subsystem->GetProvider(event.display);
			Input::Initialize(provider);
		});
		eventSubsystem->Subscribe<DisplayCloseEvent>([](const DisplayCloseEvent&)
		{
			Input::Release();
		});
	}

	void MyGame::OnInputSubsystemRemoved([[maybe_unused]] InputSubsystem* subsystem)
	{
		Input::Release();
	}

	void MyGame::OnSceneSubsystemAdded(SceneSubsystem* subsystem)
	{
		ASSERT_FMT(renderSubsystem, "렌더 서브시스템이 nullptr입니다.");

		renderSubsystem->OnRender() += std::bind_front(&SceneSubsystem::PreRender, subsystem);
		renderSubsystem->OnRender() += std::bind_front(&SceneSubsystem::Render, subsystem);
		renderSubsystem->OnRender() += std::bind_front(&SceneSubsystem::PostRender, subsystem);

		auto stageScene = std::make_unique<Scene_Stage>();
		subsystem->AddScene(std::make_unique<Scene_Title>(stageScene.get(), &sceneTransition));
		subsystem->AddScene(std::move(stageScene));
	}

	void MyGame::OnAssetSubsystemAdded(AssetSubsystem* subsystem)
	{
		ASSERT_FMT(sceneSubsystem, "월드 서브시스템이 nullptr입니다.");
		ASSERT_FMT(renderSubsystem, "렌더 서브시스템이 nullptr입니다.");

		sceneSubsystem->OnEnterScene() += std::bind_front(&AssetSubsystem::OnSceneEntered, subsystem);
		sceneSubsystem->OnExitScene() += std::bind_front(&AssetSubsystem::OnSceneExited, subsystem);

		const GPUDevice& device = renderSubsystem->GetDevice();
		subsystem->AddImporter<ModelAssetImporter>();
		subsystem->AddImporter<MeshAssetImporter>(device);
		subsystem->AddImporter<TextureAssetImporter>(device);
		subsystem->AddImporter<CubemapAssetImporter>(device);
		subsystem->AddImporter<MaterialAssetImporter>(device);
		subsystem->AddImporter<ShaderAssetImporter>();
		subsystem->AddImporter<FontAssetImporter>();
		subsystem->AddImporter<AnimationAssetImporter>();

		Assets::Initialize(subsystem);
	}

	void MyGame::OnAssetSubsystemRemoved([[maybe_unused]] AssetSubsystem* subsystem)
	{
		Assets::Release();
	}
}
