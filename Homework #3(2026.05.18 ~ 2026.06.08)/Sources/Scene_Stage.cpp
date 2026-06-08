#include "Precompiled.h"
#include "Scene_Stage.h"

#include "AnimationClip.h"
#include "Animator.h"
#include "Camera.h"
#include "CameraController.h"
#include "CubeCollider.h"
#include "EnemyController.h"
#include "GameObject.h"
#include "Light.h"
#include "Logger.h"
#include "Material.h"
#include "Model.h"
#include "PlayerController.h"
#include "Quaternion.h"
#include "ResourceSystem.h"
#include "SceneSystem.h"
#include "Terrain.h"
#include "TerrainRenderer.h"
#include "Transform.h"
#include "Vector4D.h"

void Scene_Stage::OnLoad()
{
	stageClearTriggered = false;
	initialEnemySpawnCount = GetEnemySpawnCount();

	GameObject* const playerObject{ CreatePlayer() };
	if (playerObject == nullptr)
	{
		return;
	}

	Transform* const playerTransform{ playerObject->GetComponent<Transform>() };
	if (playerTransform == nullptr)
	{
		Logger::Critical(L"[Scene_Stage] Player has no Transform.");
		return;
	}

	CreateEnemies();
	CreateLight();
	CreateTerrain();
	PlayPlayerIdleAnimation(playerObject);
	CreateCamera(playerTransform);
}

void Scene_Stage::OnUnload()
{
	stageClearTriggered = false;
	initialEnemySpawnCount = 0;
}

void Scene_Stage::OnFixedUpdate()
{
	CheckStageClear();
}

std::wstring_view Scene_Stage::GetPlayerModelPath() const noexcept
{
	return L"Resources/Models/Gunship.bin";
}

std::wstring_view Scene_Stage::GetEnemyModelPath() const noexcept
{
	return L"Resources/Models/SuperCobra.bin";
}

std::wstring_view Scene_Stage::GetPlayerAnimationPath() const noexcept
{
	return L"Resources/Animations/Idle.bin";
}

Vector3D Scene_Stage::GetPlayerColliderCenter() const noexcept
{
	return Vector3D::GetZero();
}

Vector3D Scene_Stage::GetEnemyColliderSize() const noexcept
{
	return Vector3D(6.25f, 3.75f, 17.0f);
}

Vector3D Scene_Stage::GetEnemyColliderCenter() const noexcept
{
	return Vector3D(0.125f, 0.125f, 0.0f);
}

Vector3D Scene_Stage::GetEnemySpawnerPosition() const noexcept
{
	return Vector3D(0.0f, 200.0f, 0.0f);
}

float Scene_Stage::GetEnemyMinDistanceFromPlayer() const noexcept
{
	return 140.0f;
}

ColorRGBA Scene_Stage::GetLightColor() const noexcept
{
	return ColorRGBA::GetWhite();
}

Vector3D Scene_Stage::GetCameraSpawnPosition() const noexcept
{
	return Vector3D(0.0f, 0.0f, -10.0f);
}

Vector3D Scene_Stage::GetCameraOffset() const noexcept
{
	return Vector3D(0.0f, 60.0f, -150.0f);
}

Vector3D Scene_Stage::GetFirstPersonOffset() const noexcept
{
	return Vector3D(0.0f, 10.0f, 20.0f);
}

ColorRGBA Scene_Stage::GetSkyColor() const noexcept
{
	return ColorRGBA(0.4f, 0.6f, 0.9f, 1.0f);
}

void Scene_Stage::SpawnEnemy(const Vector3D& position_)
{
	Model* const enemyPrefab{ ResourceSystem::GetInstance().GetResource<Model>(GetEnemyModelPath()) };
	if (enemyPrefab == nullptr)
	{
		Logger::Critical(L"[Scene_Stage] Spawn failed: enemy model not found. path={}", GetEnemyModelPath());
		return;
	}

	GameObject* const enemyObject{ Instantiate(enemyPrefab) };
	if (enemyObject == nullptr)
	{
		Logger::Critical(L"[Scene_Stage] Spawn failed: instantiate returned null.");
		return;
	}

	enemyObject->SetName(L"Enemy");
	enemyObject->SetTag(L"Enemy");

	auto cleanupChildren = [&](this auto& self, GameObject* current_) -> void
	{
		for (Transform* childTransform : current_->GetComponent<Transform>()->GetChildren())
		{
			GameObject* childGO = childTransform->GetOwner();
			if (childGO != nullptr)
			{
				childGO->SetTag(L"Untagged");
				self(childGO);
			}
		}
	};
	cleanupChildren(enemyObject);

	if (Transform* const enemyTransform{ enemyObject->GetComponent<Transform>() }; enemyTransform != nullptr)
	{
		enemyTransform->SetWorldPosition(position_);
		enemyTransform->SetLocalScale(Vector3D(20.0f, 20.0f, 20.0f));
	}

	enemyObject->AddComponent<EnemyController>();

	if (AnimationClip* const idleAnim{ ResourceSystem::GetInstance().GetResource<AnimationClip>(GetPlayerAnimationPath()) })
	{
		Animator* const animator{ enemyObject->AddComponent<Animator>() };
		animator->Play(idleAnim, true);
	}

	CubeCollider* const enemyCollider{ enemyObject->AddComponent<CubeCollider>() };
	enemyCollider->SetSize(GetEnemyColliderSize());
	enemyCollider->SetCenter(GetEnemyColliderCenter());
	enemyCollider->SetStatic(false);
	enemyCollider->UpdateVolume();
}

GameObject* Scene_Stage::CreatePlayer()
{
	Model* const playerPrefab{ ResourceSystem::GetInstance().GetResource<Model>(GetPlayerModelPath()) };
	if (playerPrefab == nullptr)
	{
		Logger::Critical(L"Model not found: {}", GetPlayerModelPath());
		return nullptr;
	}

	GameObject* const playerObject{ Instantiate(playerPrefab) };
	if (playerObject == nullptr)
	{
		Logger::Critical(L"[Scene_Stage] Failed to instantiate player.");
		return nullptr;
	}

	playerObject->SetTag(L"Player");

	Transform* const playerTransform{ playerObject->GetComponent<Transform>() };
	if (playerTransform == nullptr)
	{
		Logger::Critical(L"[Scene_Stage] Player has no Transform.");
		return nullptr;
	}

	playerTransform->SetWorldPosition(GetPlayerSpawnPosition());
	playerTransform->SetLocalScale(Vector3D(10.0f, 10.0f, 10.0f));
	playerObject->AddComponent<PlayerController>();

	CubeCollider* const playerCollider{ playerObject->AddComponent<CubeCollider>() };
	playerCollider->SetSize(GetPlayerColliderSize());
	playerCollider->SetCenter(GetPlayerColliderCenter());
	playerCollider->SetStatic(false);
	playerCollider->UpdateVolume();

	return playerObject;
}

void Scene_Stage::CreateEnemies()
{
	const int spawnCount{ initialEnemySpawnCount };
	if (spawnCount <= 0)
	{
		return;
	}

	Vector3D playerPos{ GetPlayerSpawnPosition() };
	const Vector3D spawnerPos{ GetEnemySpawnerPosition() };

	for (int index{ 0 }; index < spawnCount; ++index)
	{
		SpawnEnemy(GenerateSpawnPosition(spawnerPos, &playerPos));
	}
}

void Scene_Stage::CheckStageClear()
{
	if (stageClearTriggered || initialEnemySpawnCount <= 0)
	{
		return;
	}

	if (!FindObjectsWithTag(L"Enemy").empty())
	{
		return;
	}

	stageClearTriggered = true;
	MessageBoxW(nullptr, L"Game Clear!", L"Clear", MB_OK | MB_ICONINFORMATION);
	SceneSystem::GetInstance().LoadScene(L"Title");
}

void Scene_Stage::CreateLight()
{
	GameObject* const lightObject{ Instantiate() };
	lightObject->SetName(L"Directional Light");

	if (Transform* const lightTransform{ lightObject->GetComponent<Transform>() }; lightTransform != nullptr)
	{
		lightTransform->SetWorldPosition(Vector3D(0.0f, 500.0f, 0.0f));
		lightTransform->SetWorldRotation(
			Quaternion::LookRotation(Vector3D(-0.5f, -1.0f, 0.5f).GetNormalized(), Vector3D::GetUp()));
	}

	Light* const mainLight{ lightObject->AddComponent<Light>() };
	mainLight->SetColor(GetLightColor());
}

void Scene_Stage::CreateTerrain()
{
	Terrain* const terrain{ ResourceSystem::GetInstance().GetResource<Terrain>(GetTerrainPath()) };
	if (terrain == nullptr)
	{
		Logger::Warning(L"Terrain not found: {}", GetTerrainPath());
		return;
	}

	Material* const terrainMaterial{ ResourceSystem::GetInstance().GetResource<Material>(GetTerrainMaterialPath()) };
	if (terrainMaterial == nullptr)
	{
		Logger::Warning(L"Terrain material not found: {}", GetTerrainMaterialPath());
		return;
	}

	GameObject* const terrainObject{ Instantiate() };
	terrainObject->SetName(std::wstring(GetTerrainObjectName()));

	TerrainRenderer* const terrainRenderer{ terrainObject->AddComponent<TerrainRenderer>() };
	terrainRenderer->SetTerrain(terrain);
	terrainRenderer->SetMaterial(terrainMaterial);
}

void Scene_Stage::CreateCamera(Transform* playerTransform_)
{
	GameObject* const cameraObject{ Instantiate(GetCameraSpawnPosition()) };
	cameraObject->SetName(L"Main Camera");
	cameraObject->SetTag(L"MainCamera");

	Camera* const camera{ cameraObject->AddComponent<Camera>() };
	camera->SetViewport(Vector4D(0.0f, 0.0f, 1.0f, 1.0f));
	camera->SetFOV(60.0f);
	camera->SetNearClipPlane(0.1f);
	camera->SetFarClipPlane(1000.0f);
	camera->SetClearColor(GetSkyColor());

	CameraController* const cameraController{ cameraObject->AddComponent<CameraController>() };
	cameraController->SetTarget(playerTransform_);
	cameraController->SetOffset(GetCameraOffset());
	cameraController->SetFirstPersonOffset(GetFirstPersonOffset());
}

void Scene_Stage::PlayPlayerIdleAnimation(GameObject* playerObject_)
{
	if (playerObject_ == nullptr)
	{
		return;
	}

	AnimationClip* const playerIdleAnim{ ResourceSystem::GetInstance().GetResource<AnimationClip>(GetPlayerAnimationPath()) };
	if (playerIdleAnim == nullptr)
	{
		Logger::Warning(L"Animation not found: {}", GetPlayerAnimationPath());
		return;
	}

	Animator* const animator{ playerObject_->AddComponent<Animator>() };
	animator->Play(playerIdleAnim, true);
}

Vector3D Scene_Stage::GenerateSpawnPosition(const Vector3D& center_, const Vector3D* playerPosition_) noexcept
{
	const Vector3D halfExtents{ GetEnemySpawnHalfExtents() };
	const float minDistance{ GetEnemyMinDistanceFromPlayer() };

	std::uniform_real_distribution<float> xDistribution(-halfExtents.x, halfExtents.x);
	std::uniform_real_distribution<float> zDistribution(-halfExtents.z, halfExtents.z);

	Vector3D spawnPosition{ center_ };
	Vector3D farthestPosition{ center_ };
	float farthestSqrDistance{ -1.0f };

	for (int attempts{ 0 }; attempts < 64; ++attempts)
	{
		spawnPosition = Vector3D(
			center_.x + xDistribution(randomEngine),
			center_.y,
			center_.z + zDistribution(randomEngine));

		if (playerPosition_ == nullptr)
		{
			return spawnPosition;
		}

		const Vector3D horizontalOffset{
			spawnPosition.x - playerPosition_->x,
			0.0f,
			spawnPosition.z - playerPosition_->z };

		const float sqrDistance{ horizontalOffset.GetSqrMagnitude() };
		if (sqrDistance > farthestSqrDistance)
		{
			farthestSqrDistance = sqrDistance;
			farthestPosition = spawnPosition;
		}

		if (horizontalOffset.GetMagnitude() >= minDistance)
		{
			return spawnPosition;
		}
	}

	const Vector3D candidates[4]
	{
		Vector3D(center_.x - halfExtents.x, center_.y, center_.z - halfExtents.z),
		Vector3D(center_.x - halfExtents.x, center_.y, center_.z + halfExtents.z),
		Vector3D(center_.x + halfExtents.x, center_.y, center_.z - halfExtents.z),
		Vector3D(center_.x + halfExtents.x, center_.y, center_.z + halfExtents.z)
	};

	for (const Vector3D& candidate : candidates)
	{
		const Vector3D horizontalOffset{
			candidate.x - playerPosition_->x,
			0.0f,
			candidate.z - playerPosition_->z };

		const float sqrDistance{ horizontalOffset.GetSqrMagnitude() };
		if (sqrDistance > farthestSqrDistance)
		{
			farthestSqrDistance = sqrDistance;
			farthestPosition = candidate;
		}
	}

	return farthestPosition;
}
