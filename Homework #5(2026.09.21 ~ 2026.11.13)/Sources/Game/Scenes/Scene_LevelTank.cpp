#include "Precompiled.hpp"
#include "Scene_LevelTank.hpp"

#include "Framework/Core.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Objects.hpp"
#include "Framework/Resources.hpp"
#include "Framework/UI.hpp"

#include "../Core/Assets.hpp"
#include "../Core/Input.hpp"
#include "../Core/SceneManager.hpp"
#include "../Core/Time.hpp"

#include "../Components/EnemyTankController.hpp"
#include "../Components/PlayerCameraController.hpp"
#include "../Components/PlayerTankController.hpp"

namespace TUK::Game
{
	void Scene_LevelTank::OnEnter()
	{
		enemyCount = 0;
		totalEnemyCount = 10;
		maxPlayerHealth = 10;
		isGameOver = false;
		hpTextView = nullptr;
		leftTextView = nullptr;

		Terrain* terrain = nullptr;
		{
			GameObject* terrainGameObject = Instantiate();
			terrainGameObject->SetName("Terrain");
			terrainGameObject->SetTag("Terrain");

			Transform* terrainTransform = terrainGameObject->AddComponent<Transform>();
			terrainTransform->SetLocalPosition(Vector3D<f32>::GetZero());
			terrainTransform->SetLocalRotation({});
			terrainTransform->SetLocalScale(Vector3D<f32>::GetOne());

			terrain = terrainGameObject->AddComponent<Terrain>();
			terrain->SetTerrain(Assets::LoadAsset<TerrainAsset>("Resources/Terrains/Terrain_LevelTank.tukterrain"));

			MeshRenderer* terrainRenderer = terrainGameObject->AddComponent<MeshRenderer>();
			terrainRenderer->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/ForrestGround.tukmat"));

			terrainGameObject->AddComponent<TerrainCollider>();
		}

		const f32 terrainHeight = (terrain && terrain->IsValid())
			? terrain->SampleHeightAtWorldPosition(Vector3D<f32>::GetZero())
			: 0.0f;

		PlayerCameraController* cameraController = nullptr;
		{
			GameObject* cameraGameObject = Instantiate();
			cameraGameObject->SetName("Camera");
			cameraGameObject->SetTag("Main Camera");

			Transform* cameraTransform = cameraGameObject->AddComponent<Transform>();
			cameraTransform->SetLocalRotation(Quaternion<f32>{});
			cameraTransform->SetLocalPosition(Vector3D<f32>(0.0f, terrainHeight + 3.0f, -10.0f));

			const DisplaySubsystem* displays = Engine::GetInstance().GetSubsystem<DisplaySubsystem>();
			ASSERT_FMT(displays, "디스플레이 서브시스템을 가져오지 못했습니다.");

			const Display* display = displays->GetDisplay(0);
			ASSERT_FMT(display, "타이틀 화면을 표시할 디스플레이가 없습니다.");

			const Vector2D<i32> clientSize = display->GetClientSize();
			Input::SetCursorBoundary(D3D12_RECT{ 0, 0, clientSize.GetX(), clientSize.GetY() });
			Input::EnableCursorBlock(true);

			Camera* camera = cameraGameObject->AddComponent<Camera>();
			camera->SetViewport(CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<f32>(clientSize.GetX()), static_cast<f32>(clientSize.GetY())));
			camera->SetScissorRect(CD3DX12_RECT(0, 0, clientSize.GetX(), clientSize.GetY()));
			camera->SetClearColor(ColorRGBA<f32>::GetBlue());

			cameraController = cameraGameObject->AddComponent<PlayerCameraController>();
			cameraController->SetFirstPersonOffset(Vector3D<f32>(0.0f, 3.3f, 0.5f));
		}

		{
			GameObject* skybox = Instantiate();
			skybox->SetName("Skybox Day");
			skybox->SetTag("Skybox");

			SkyboxRenderer* skyboxRenderer = skybox->AddComponent<SkyboxRenderer>();
			skyboxRenderer->SetCubemap(Assets::LoadAsset<CubemapAsset>("Resources/Cubemaps/Polyverse Skies - Blue Sky/Polyverse Skies - Blue Sky.dds"));
			skyboxRenderer->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/Skybox.tukmat"));
		}

		{
			GameObject* lightGameObject = Instantiate();
			lightGameObject->SetName("Directional Light");
			lightGameObject->SetTag("Light");

			Transform* lightTransform = lightGameObject->AddComponent<Transform>();
			lightTransform->SetLocalPosition(Vector3D<f32>(0.0f, terrainHeight + 100.0f, 0.0f));
			lightTransform->SetLocalRotation(Quaternion<f32>::Euler(55.0f, -45.0f, 0.0f));

			DirectionalLight* light = lightGameObject->AddComponent<DirectionalLight>();
			light->SetColor(ColorRGBA<f32>(1.0f, 0.98f, 0.92f, 1.0f));
			light->SetIntensity(1.2f);
			light->SetAmbientColor(ColorRGBA<f32>(0.18f, 0.20f, 0.24f, 1.0f));
		}

		{
			GameObject* playerObject = Instantiate(Assets::LoadAsset<ModelAsset>("Resources/Models/E_50.tukmodel"));
			playerObject->SetName("Tank");
			playerObject->SetTag("Player");

			Transform* playerTransform = playerObject->GetComponent<Transform>();
			playerTransform->SetLocalPosition(Vector3D<f32>(0.0f, terrainHeight, 0.0f));
			playerTransform->SetLocalRotation(Quaternion<f32>{});
			playerTransform->SetLocalScale(Vector3D<f32>(1.0f, 1.0f, 1.0f));

			[[maybe_unused]] PlayerTankController* playerController = playerObject->AddComponent<PlayerTankController>();

			cameraController->SetTarget(playerTransform);

			BoxCollider* playerCollider = playerObject->AddComponent<BoxCollider>();
			playerCollider->SetCenter(Vector3D<f32>(0.0f, 1.0f, 0.0f));
			playerCollider->SetSize(Vector3D<f32>(2.4f, 2.0f, 3.6f));
		}

		{
			constexpr i32 ENEMY_COUNT = 10;
			constexpr i32 COLUMN_COUNT = 10;
			constexpr i32 ROW_COUNT = ENEMY_COUNT / COLUMN_COUNT;

			const f32 spawnWidth = (terrain && terrain->IsValid()) ? terrain->GetLocalWidth() * 0.8f : 200.0f;
			const f32 spawnDepth = (terrain && terrain->IsValid()) ? terrain->GetLocalDepth() * 0.8f : 200.0f;
			const f32 columnWidth = spawnWidth / static_cast<f32>(COLUMN_COUNT);
			const f32 rowDepth = spawnDepth / static_cast<f32>(ROW_COUNT);

			const ModelAsset* enemyModel = Assets::LoadAsset<ModelAsset>("Resources/Models/Panzer_VI_E.tukmodel");

			for (i32 index = 0; index < ENEMY_COUNT; ++index)
			{
				GameObject* enemyObject = Instantiate(enemyModel);
				ASSERT_FMT(enemyObject, "적군 모델을 생성하지 못했습니다.");

				enemyObject->SetName(std::format("Enemy Tank {}", index + 1));
				enemyObject->SetTag("Enemy");

				const f32 x = -spawnWidth * 0.5f + (static_cast<f32>(index % COLUMN_COUNT) + Random::RandFloat(0.25f, 0.75f)) * columnWidth;
				const f32 z = -spawnDepth * 0.5f + (static_cast<f32>(index / COLUMN_COUNT) + Random::RandFloat(0.25f, 0.75f)) * rowDepth;
				Vector3D<f32> enemyPosition(x, 0.0f, z);
				const f32 enemyTerrainHeight = (terrain && terrain->IsValid())
					? terrain->SampleHeightAtWorldPosition(enemyPosition)
					: 0.0f;
				enemyPosition.SetY(enemyTerrainHeight);

				Transform* enemyTransform = enemyObject->GetComponent<Transform>();
				enemyTransform->SetLocalPosition(enemyPosition);
				enemyTransform->SetLocalRotation(Quaternion<f32>{});
				enemyTransform->SetLocalScale(Vector3D<f32>(2.0f, 2.0f, 2.0f));

				[[maybe_unused]] EnemyTankController* enemyController = enemyObject->AddComponent<EnemyTankController>();
				++enemyCount;

				BoxCollider* enemyCollider = enemyObject->AddComponent<BoxCollider>();
				enemyCollider->SetCenter(Vector3D<f32>(0.0f, 1.5f, 0.0f));
				enemyCollider->SetSize(Vector3D<f32>(4.2f, 3.2f, 6.8f));
			}

			totalEnemyCount = ENEMY_COUNT;
		}

		{
			constexpr f32 HUD_WIDTH = 300.0f;
			constexpr f32 HUD_ROW_HEIGHT = 32.0f;
			constexpr f32 HUD_ROW_GAP = 10.0f;
			constexpr f32 HUD_MARGIN_X = 14.0f;
			constexpr f32 HUD_MARGIN_Y = 14.0f;

			const DisplaySubsystem* displays = Engine::GetInstance().GetSubsystem<DisplaySubsystem>();
			const Display* display = displays ? displays->GetDisplay(0) : nullptr;
			const Vector2D<i32> clientSize = display ? display->GetClientSize() : Vector2D<i32>(1280, 960);
			const f32 hudX = -static_cast<f32>(clientSize.GetX()) * 0.5f + HUD_WIDTH * 0.5f + HUD_MARGIN_X;
			const f32 firstRowY = -static_cast<f32>(clientSize.GetY()) * 0.5f + HUD_ROW_HEIGHT * 0.5f + HUD_MARGIN_Y;

			const auto createHudTextView = [this, hudX](std::string_view name, f32 y, std::string text) -> TextView*
			{
				GameObject* hudGameObject = Instantiate();
				hudGameObject->SetName(name);

				Transform* hudTransform = hudGameObject->AddComponent<Transform>();
				hudTransform->SetLocalPosition(Vector3D<f32>(hudX, y, 0.0f));
				hudTransform->SetLocalRotation(Quaternion<f32>{});
				hudTransform->SetLocalScale(Vector3D<f32>(HUD_WIDTH, HUD_ROW_HEIGHT, 1.0f));

				TextView* textView = hudGameObject->AddComponent<TextView>();
				textView->SetHorizontalAlignment(TextHorizontalAlignment::LEFT);
				textView->SetTexture(Assets::LoadAsset<TextureAsset>("Resources/Fonts/BMDOHYEON_ttf_0.dds"));
				textView->SetFont(Assets::LoadAsset<BitmapFontAsset>("Resources/Fonts/BMDOHYEON_ttf.tukfont"));
				textView->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/TextUI.tukmat"));
				textView->SetFontSize(28.0f);
				textView->SetColor(ColorRGBA<f32>::GetWhite());
				textView->SetText(std::move(text));
				return textView;
			};

			hpTextView = createHudTextView("HP Text", firstRowY, std::format("HP: {}/{}", maxPlayerHealth, maxPlayerHealth));
			leftTextView = createHudTextView("Left Text", firstRowY + HUD_ROW_HEIGHT + HUD_ROW_GAP, std::format("LEFT: {}/{}", totalEnemyCount, totalEnemyCount));
		}

		if (InputAction* cancelAction = Input::GetAction("Cancel"))
		{
			cancelListenerId = cancelAction->OnStarted().AddListener([](const InputActionContext&)
			{
				SceneManager::LoadScene("Title");
			});
		}
	}

	void Scene_LevelTank::OnLateUpdate()
	{
		Input::UpdateCursorClip();

		enemyCount = 0;
		for (const GameObject* gameObject : GetObjects())
		{
			if (!gameObject->IsDestroyed() && gameObject->CompareTag("Enemy"))
			{
				++enemyCount;
			}
		}

		i32 currentPlayerHealth = 0;
		if (const GameObject* playerObject = FindGameObjectByTag("Player"))
		{
			if (const PlayerTankController* controller = playerObject->GetComponent<PlayerTankController>())
			{
				currentPlayerHealth = controller->GetHealth();
			}
		}

		if (hpTextView)
		{
			hpTextView->SetText(std::format("HP: {}/{}", currentPlayerHealth, maxPlayerHealth));
		}
		if (leftTextView)
		{
			leftTextView->SetText(std::format("LEFT: {}/{}", enemyCount, totalEnemyCount));
		}

		if (!FindGameObjectByTag("Player"))
		{
			for (GameObject* gameObject : GetObjects())
			{
				if (PlayerCameraController* camera = gameObject->GetComponent<PlayerCameraController>())
				{
					camera->SetTarget(nullptr);
				}
			}

			if (!isGameOver)
			{
				isGameOver = SceneManager::LoadScene("Game Over", 0.0f);
			}
			return;
		}

		if (isGameOver)
		{
			return;
		}

		if (enemyCount == 0)
		{
			isGameOver = SceneManager::LoadScene("Game Clear");
			return;
		}
	}

	void Scene_LevelTank::OnExit()
	{
		Input::EnableCursorBlock(false);

		if (cancelListenerId != -1)
		{
			if (InputAction* cancelAction = Input::GetAction("Cancel"))
			{
				cancelAction->OnStarted().RemoveListener(cancelListenerId);
			}

			cancelListenerId = -1;
		}
	}
}
