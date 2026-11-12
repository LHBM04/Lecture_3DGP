#include "Precompiled.hpp"
#include "Scene_LevelJet.hpp"

#include "Framework/Core.hpp"
#include "Framework/Graphics.hpp"
#include "Framework/Objects.hpp"
#include "Framework/Platform.hpp"
#include "Framework/Resources.hpp"
#include "Framework/UI.hpp"

#include "../Core/Assets.hpp"
#include "../Core/Input.hpp"
#include "../Core/SceneManager.hpp"

#include "../Components/EnemyJetController.hpp"
#include "../Components/PlayerCameraController.hpp"
#include "../Components/PlayerJetController.hpp"

namespace TUK::Game
{
	void Scene_LevelJet::OnEnter()
	{
		enemyCount = 0;
		totalEnemyCount = 10;
		maxPlayerHealth = 10;
		isGameOver = false;
		hudTextView = nullptr;

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
			terrain->SetTerrain(Assets::LoadAsset<TerrainAsset>("Resources/Terrains/Terrain_LevelJet.tukterrain"));

			MeshRenderer* terrainRenderer = terrainGameObject->AddComponent<MeshRenderer>();
			terrainRenderer->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/RockyTerrain.tukmat"));

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
			cameraTransform->SetLocalPosition(Vector3D<f32>(0.0f, terrainHeight + 30.0f, -10.0f));

			const DisplaySubsystem* displays = Engine::GetInstance().GetSubsystem<DisplaySubsystem>();
			ASSERT_FMT(displays, "디스플레이 서브시스템을 가져오지 못했습니다.");

			const Display* display = displays->GetDisplay(0);
			ASSERT_FMT(display, "비행기 레벨을 표시할 디스플레이가 없습니다.");

			const Vector2D<i32> clientSize = display->GetClientSize();
			Input::SetCursorBoundary(D3D12_RECT{ 0, 0, clientSize.GetX(), clientSize.GetY() });
			Input::EnableCursorBlock(true);

			Camera* camera = cameraGameObject->AddComponent<Camera>();
			camera->SetViewport(CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<f32>(clientSize.GetX()), static_cast<f32>(clientSize.GetY())));
			camera->SetScissorRect(CD3DX12_RECT(0, 0, clientSize.GetX(), clientSize.GetY()));
			camera->SetClearColor(ColorRGBA<f32>::GetBlue());

			cameraController = cameraGameObject->AddComponent<PlayerCameraController>();
		}

		{
			GameObject* skybox = Instantiate();
			skybox->SetName("Skybox Night");
			skybox->SetTag("Skybox");

			SkyboxRenderer* skyboxRenderer = skybox->AddComponent<SkyboxRenderer>();
			skyboxRenderer->SetCubemap(Assets::LoadAsset<CubemapAsset>("Resources/Cubemaps/Polyverse Skies - Night Sky/Polyverse Skies - Night Sky.dds"));
			skyboxRenderer->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/Skybox.tukmat"));
		}

		{
			GameObject* lightGameObject = Instantiate();
			lightGameObject->SetName("Directional Light");
			lightGameObject->SetTag("Light");

			Transform* lightTransform = lightGameObject->AddComponent<Transform>();
			lightTransform->SetLocalPosition(Vector3D<f32>(0.0f, terrainHeight + 200.0f, 0.0f));
			lightTransform->SetLocalRotation(Quaternion<f32>::Euler(65.0f, 135.0f, 0.0f));

			DirectionalLight* light = lightGameObject->AddComponent<DirectionalLight>();
			light->SetColor(ColorRGBA<f32>(0.65f, 0.78f, 1.0f, 1.0f));
			light->SetIntensity(0.8f);
			light->SetAmbientColor(ColorRGBA<f32>(0.08f, 0.10f, 0.16f, 1.0f));
		}

		{
			GameObject* playerObject = Instantiate(Assets::LoadAsset<ModelAsset>("Resources/Models/StarSparrow1.tukmodel"));
			ASSERT_FMT(playerObject, "플레이어 제트 모델을 생성하지 못했습니다.");

			playerObject->SetName("Jet");
			playerObject->SetTag("Player");

			Transform* playerTransform = playerObject->GetComponent<Transform>();
			playerTransform->SetLocalPosition(Vector3D<f32>(0.0f, terrainHeight + 30.0f, 0.0f));
			playerTransform->SetLocalRotation(Quaternion<f32>{});
			playerTransform->SetLocalScale(Vector3D<f32>(1.0f, 1.0f, 1.0f));

			[[maybe_unused]] PlayerJetController* playerController = playerObject->AddComponent<PlayerJetController>();

			cameraController->SetTarget(playerTransform);

			BoxCollider* playerCollider = playerObject->AddComponent<BoxCollider>();
			playerCollider->SetSize(Vector3D<f32>(2.0f, 1.0f, 2.0f));
		}

		{
			constexpr i32 ENEMY_COUNT = 10;
			constexpr i32 COLUMN_COUNT = 10;
			constexpr i32 ROW_COUNT = ENEMY_COUNT / COLUMN_COUNT;

			const f32 spawnWidth = (terrain && terrain->IsValid()) ? terrain->GetLocalWidth() * 0.8f : 200.0f;
			const f32 spawnDepth = (terrain && terrain->IsValid()) ? terrain->GetLocalDepth() * 0.8f : 200.0f;
			const f32 columnWidth = spawnWidth / static_cast<f32>(COLUMN_COUNT);
			const f32 rowDepth = spawnDepth / static_cast<f32>(ROW_COUNT);

			const ModelAsset* enemyModel = Assets::LoadAsset<ModelAsset>("Resources/Models/StarSparrow40.tukmodel");

			for (i32 index = 0; index < ENEMY_COUNT; ++index)
			{
				GameObject* enemyObject = Instantiate(enemyModel);
				ASSERT_FMT(enemyObject, "적군 모델을 생성하지 못했습니다.");

				enemyObject->SetName(std::format("Enemy Jet {}", index + 1));
				enemyObject->SetTag("Enemy");

				const f32 x = -spawnWidth * 0.5f + (static_cast<f32>(index % COLUMN_COUNT) + Random::RandFloat(0.25f, 0.75f)) * columnWidth;
				const f32 z = -spawnDepth * 0.5f + (static_cast<f32>(index / COLUMN_COUNT) + Random::RandFloat(0.25f, 0.75f)) * rowDepth;
				Vector3D<f32> enemyPosition(x, 0.0f, z);
				const f32 enemyTerrainHeight = (terrain && terrain->IsValid())
					? terrain->SampleHeightAtWorldPosition(enemyPosition)
					: 0.0f;
				enemyPosition.SetY(enemyTerrainHeight + 30.0f);

				Transform* enemyTransform = enemyObject->GetComponent<Transform>();
				enemyTransform->SetLocalPosition(enemyPosition);
				enemyTransform->SetLocalRotation(Quaternion<f32>{});
				enemyTransform->SetLocalScale(Vector3D<f32>(1.0f, 1.0f, 1.0f));

				[[maybe_unused]] EnemyJetController* enemyController = enemyObject->AddComponent<EnemyJetController>();
				++enemyCount;

				BoxCollider* enemyCollider = enemyObject->AddComponent<BoxCollider>();
				enemyCollider->SetSize(Vector3D<f32>(11.0f, 5.0f, 12.0f));
			}

			totalEnemyCount = ENEMY_COUNT;
		}

		{
			const DisplaySubsystem* displays = Engine::GetInstance().GetSubsystem<DisplaySubsystem>();
			const Display* display = displays ? displays->GetDisplay(0) : nullptr;
			const Vector2D<i32> clientSize = display ? display->GetClientSize() : Vector2D<i32>(1280, 960);
			const f32 hudX = -static_cast<f32>(clientSize.GetX()) * 0.5f + 30.0f;
			const f32 hudY = -static_cast<f32>(clientSize.GetY()) * 0.5f + 40.0f;

			GameObject* hudGameObject = Instantiate();
			hudGameObject->SetName("HUD Text");

			Transform* hudTransform = hudGameObject->AddComponent<Transform>();
			hudTransform->SetLocalPosition(Vector3D<f32>(hudX, hudY, 0.0f));
			hudTransform->SetLocalRotation(Quaternion<f32>{});
			hudTransform->SetLocalScale(Vector3D<f32>(300.0f, 70.0f, 1.0f));

			hudTextView = hudGameObject->AddComponent<TextView>();
			hudTextView->SetHorizontalAlignment(TextHorizontalAlignment::LEFT);
			hudTextView->SetTexture(Assets::LoadAsset<TextureAsset>("Resources/Fonts/BMDOHYEON_ttf_0.dds"));
			hudTextView->SetFont(Assets::LoadAsset<BitmapFontAsset>("Resources/Fonts/BMDOHYEON_ttf.tukfont"));
			hudTextView->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/TextUI.tukmat"));
			hudTextView->SetFontSize(28.0f);
			hudTextView->SetColor(ColorRGBA<f32>::GetWhite());
			hudTextView->SetText(std::format("HP: {}/{}\nLEFT: {}/{}", maxPlayerHealth, maxPlayerHealth, totalEnemyCount, totalEnemyCount));
		}

		if (InputAction* cancelAction = Input::GetAction("Cancel"))
		{
			returnListenerId = cancelAction->OnStarted().AddListener([](const InputActionContext&)
			{
				SceneManager::LoadScene("Title");
			});
		}
	}

	void Scene_LevelJet::OnLateUpdate()
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
			if (const PlayerJetController* controller = playerObject->GetComponent<PlayerJetController>())
			{
				currentPlayerHealth = controller->GetHealth();
			}
		}

		if (hudTextView)
		{
			hudTextView->SetText(std::format("HP: {}/{}\nLEFT: {}/{}", currentPlayerHealth, maxPlayerHealth, enemyCount, totalEnemyCount));
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
				LOG_INFO("LevelJet 게임 오버: 플레이어 오브젝트를 찾을 수 없습니다.");
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

		const GameObject* terrainObject = FindGameObjectByTag("Terrain");
		const TerrainCollider* terrainCollider = terrainObject ? terrainObject->GetComponent<TerrainCollider>() : nullptr;
		if (!terrainCollider)
		{
			return;
		}

		for (GameObject* gameObject : GetObjects())
		{
			if (gameObject->IsDestroyed() || !gameObject->IsEnabled())
			{
				continue;
			}

			if (!gameObject->CompareTag("Player"))
			{
				continue;
			}

			const BoxCollider* collider = gameObject->GetComponent<BoxCollider>();
			if (terrainCollider->Intersects(collider))
			{
				LOG_INFO("LevelJet 게임 오버: 지형 충돌 발생 (오브젝트 이름={}, 태그={})", gameObject->GetName(), gameObject->GetTag());
				gameObject->Destroy();
				isGameOver = SceneManager::LoadScene("Game Over", 0.0f);
				return;
			}
		}
	}

	void Scene_LevelJet::OnExit()
	{
		Input::EnableCursorBlock(false);

		if (returnListenerId != -1)
		{
			if (InputAction* cancelAction = Input::GetAction("Cancel"))
			{
				cancelAction->OnStarted().RemoveListener(returnListenerId);
			}

			returnListenerId = -1;
		}
	}
}
