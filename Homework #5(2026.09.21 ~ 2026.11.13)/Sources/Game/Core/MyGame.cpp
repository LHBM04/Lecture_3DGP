#include "Precompiled.hpp"
#include "MyGame.hpp"

#include "Framework/Core.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Objects.hpp"
#include "Framework/Platform.hpp"
#include "Framework/Resources.hpp"

#include "../Scenes/Scene_LevelJet.hpp"
#include "../Scenes/Scene_LevelClear.hpp"
#include "../Scenes/Scene_LevelFail.hpp"
#include "../Scenes/Scene_LevelTank.hpp"
#include "../Scenes/Scene_Title.hpp"

#include "Assets.hpp"
#include "Input.hpp"
#include "SceneManager.hpp"
#include "Time.hpp"

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
		OnSubsystemRemoved<SceneSubsystem>(this, &MyGame::OnSceneSubsystemRemoved);
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
		options.size = Vector2D(800, 600);
		options.style = WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
		options.styleEx = WS_EX_APPWINDOW;
		options.show = SW_SHOW;

		auto result = displaySubsystem->CreateDisplay(options);
		if (!result)
		{
			LOG_ERROR("디스플레이 생성에 실패했습니다: {}", result.error());
			return SystemStatus::FAILURE;
		}

		result.value()->SetClientSize(options.size);

		return App::Startup();
	}

	SystemStatus MyGame::Routine()
	{
		return App::Routine();
	}

	void MyGame::Shutdown()
	{
		App::Shutdown();
	}

	void MyGame::OnInputSubsystemAdded(InputSubsystem* subsystem)
	{
		ASSERT_FMT(eventSubsystem, "이벤트 서브시스템이 nullptr입니다.");

		eventSubsystem->Subscribe<InputActivationEvent>(subsystem, &InputSubsystem::OnInputActivation);
		eventSubsystem->Subscribe<DisplayCreateEvent>(subsystem, &InputSubsystem::OnDisplayCreate);
		eventSubsystem->Subscribe<DisplayCloseEvent>(subsystem, &InputSubsystem::OnDisplayClose);
		eventSubsystem->Subscribe<DisplayFocusEvent>(subsystem, &InputSubsystem::OnDisplayFocus);

		if (InputAction* moveAction = subsystem->AddAction("Move"))
		{
			moveAction->AddVector2DBinding(KeyCode::UP, KeyCode::DOWN, KeyCode::LEFT, KeyCode::RIGHT);
			moveAction->AddVector2DBinding(KeyCode::W, KeyCode::S, KeyCode::A, KeyCode::D);
		}

		if (InputAction* cancelAction = subsystem->AddAction("Cancel"))
		{
			cancelAction->AddButtonBinding(KeyCode::ESCAPE);
		}

		Input::Initialize(subsystem);
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

		SceneManager::Initialize(subsystem);

		SceneManager::AddScene("Title", std::make_unique<Scene_Title>());
		SceneManager::AddScene("Stage Jet", std::make_unique<Scene_LevelJet>());
		SceneManager::AddScene("Stage Tank", std::make_unique<Scene_LevelTank>());
		SceneManager::AddScene("Game Over", std::make_unique<Scene_LevelFail>());
		SceneManager::AddScene("Game Clear", std::make_unique<Scene_LevelClear>());
	}

	void MyGame::OnSceneSubsystemRemoved([[maybe_unused]] SceneSubsystem* subsystem)
	{
		SceneManager::Release();
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
		subsystem->AddImporter<BitmapFontAssetImporter>();
		subsystem->AddImporter<AnimationAssetImporter>();
		subsystem->AddImporter<TerrainAssetImporter>(device);

		Assets::Initialize(subsystem);
	}

	void MyGame::OnAssetSubsystemRemoved([[maybe_unused]] AssetSubsystem* subsystem)
	{
		Assets::Release();
	}
}
