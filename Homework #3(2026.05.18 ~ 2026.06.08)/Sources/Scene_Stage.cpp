#include "Precompiled.h"
#include "Scene_Stage.h"

#include <algorithm>

#include "AnimationClip.h"
#include "Animator.h"
#include "Camera.h"
#include "CameraController.h"
#include "CubeCollider.h"
#include "EnemyController.h"
#include "GameObject.h"
#include "InputSystem.h"
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

	CreateTerrain();

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

	CreateLight();
	CreateEnemies();
	PlayPlayerIdleAnimation(playerObject);
	CreateCamera(playerTransform);
}

void Scene_Stage::OnUnload()
{
	stageClearTriggered = false;
	initialEnemySpawnCount = 0;
}

void Scene_Stage::OnUpdate()
{
	if (InputSystem::GetInstance().IsKeyPressed(KeyCode::LeftControl))
	{
		DestroyOneEnemyForDebug();
	}
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
	return Vector3D(0.0f, 30.0f, 0.0f);
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
	return Vector3D(0.0f, 10.0f, -25.0f);
}

Vector3D Scene_Stage::GetFirstPersonOffset() const noexcept
{
	return Vector3D(0.0f, 4.5f, 12.0f);
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
		enemyTransform->SetWorldPosition(ResolveEnemySpawnPosition(position_));
		enemyTransform->SetLocalScale(Vector3D::GetOne());
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

	playerTransform->SetWorldPosition(ResolvePlayerSpawnPosition());
	playerTransform->SetLocalScale(Vector3D::GetOne());
	playerObject->AddComponent<PlayerController>();

	CubeCollider* const playerCollider{ playerObject->AddComponent<CubeCollider>() };
	playerCollider->SetSize(GetPlayerColliderSize());
	playerCollider->SetCenter(GetPlayerColliderCenter());
	playerCollider->SetStatic(false);
	playerCollider->UpdateVolume();

	return playerObject;
}

Vector3D Scene_Stage::ResolvePlayerSpawnPosition() const
{
	Vector3D spawnPosition{ GetPlayerSpawnPosition() };

	Terrain* const terrain{ ResourceSystem::GetInstance().GetResource<Terrain>(GetTerrainPath()) };
	if (terrain == nullptr || !terrain->ContainsLocalPosition(spawnPosition.x, spawnPosition.z))
	{
		return spawnPosition;
	}

	const float terrainHeight{ terrain->SampleHeightAtLocalPosition(spawnPosition.x, spawnPosition.z) };
	const Vector3D colliderSize{ GetPlayerColliderSize() };
	const Vector3D colliderCenter{ GetPlayerColliderCenter() };
	const float colliderBottomOffset{ colliderCenter.y - (colliderSize.y * 0.5f) };
	const float minimumVisualClearance{ 22.0f };
	const float minimumSafeY{ terrainHeight - colliderBottomOffset + minimumVisualClearance };

	if (spawnPosition.y < minimumSafeY)
	{
		spawnPosition.y = minimumSafeY;
	}

	return spawnPosition;
}

Vector3D Scene_Stage::ResolveEnemySpawnPosition(Vector3D spawnPosition_) const
{
	Terrain* const terrain{ ResourceSystem::GetInstance().GetResource<Terrain>(GetTerrainPath()) };
	if (terrain == nullptr || !terrain->ContainsLocalPosition(spawnPosition_.x, spawnPosition_.z))
	{
		return spawnPosition_;
	}

	const float terrainHeight{ terrain->SampleHeightAtLocalPosition(spawnPosition_.x, spawnPosition_.z) };
	const Vector3D colliderSize{ GetEnemyColliderSize() };
	const Vector3D colliderCenter{ GetEnemyColliderCenter() };
	const float colliderBottomOffset{ colliderCenter.y - (colliderSize.y * 0.5f) };
	const float minimumVisualClearance{ 24.0f };
	const float minimumSafeY{ terrainHeight - colliderBottomOffset + minimumVisualClearance };

	if (spawnPosition_.y < minimumSafeY)
	{
		spawnPosition_.y = minimumSafeY;
	}

	return spawnPosition_;
}

void Scene_Stage::CreateEnemies()
{
	const int spawnCount{ initialEnemySpawnCount };
	if (spawnCount <= 0)
	{
		return;
	}

	Vector3D playerPos{ ResolvePlayerSpawnPosition() };
	const Vector3D spawnerPos{ GetEnemySpawnerPosition() };
	const std::vector<Vector3D> spawnPositions{ GenerateEnemySpawnPositions(spawnerPos, playerPos, spawnCount) };

	for (const Vector3D& spawnPosition : spawnPositions)
	{
		SpawnEnemy(spawnPosition);
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

void Scene_Stage::DestroyOneEnemyForDebug()
{
	const std::vector<GameObject*> enemies{ FindObjectsWithTag(L"Enemy") };
	if (enemies.empty())
	{
		return;
	}

	Destroy(enemies.front());
	Logger::Info(L"[Scene_Stage] Debug destroyed one enemy. remaining={}", enemies.size() - 1);
}

void Scene_Stage::CreateLight()
{
	GameObject* const lightObject{ Instantiate() };
	lightObject->SetName(L"Directional Light");

	if (Transform* const lightTransform{ lightObject->GetComponent<Transform>() }; lightTransform != nullptr)
	{
		const Vector3D lightDirection{ GetLightDirection().GetNormalized() };
		lightTransform->SetWorldPosition(lightDirection * -200.0f);
		lightTransform->SetWorldRotation(Quaternion::LookRotation(lightDirection, Vector3D::GetUp()));
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

	if (Transform* const terrainTransform{ terrainObject->GetComponent<Transform>() }; terrainTransform != nullptr)
	{
		terrainTransform->SetWorldPosition(Vector3D::GetZero());
	}

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

std::vector<Vector3D> Scene_Stage::GenerateEnemySpawnPositions(
	const Vector3D& center_,
	const Vector3D& playerPosition_,
	int spawnCount_)
{
	std::vector<Vector3D> spawnPositions{};
	if (spawnCount_ <= 0)
	{
		return spawnPositions;
	}

	const Vector3D halfExtents{ GetEnemySpawnHalfExtents() };
	const float minDistance{ GetEnemyMinDistanceFromPlayer() };
	const float minSqrDistance{ minDistance * minDistance };
	const int gridColumns{ static_cast<int>(std::ceil(std::sqrt(static_cast<float>(spawnCount_) * 2.0f))) };
	const int gridRows{ gridColumns };
	const float cellWidth{ (halfExtents.x * 2.0f) / static_cast<float>(gridColumns) };
	const float cellDepth{ (halfExtents.z * 2.0f) / static_cast<float>(gridRows) };
	const float jitterX{ cellWidth * 0.28f };
	const float jitterZ{ cellDepth * 0.28f };

	std::uniform_real_distribution<float> xJitterDistribution(-jitterX, jitterX);
	std::uniform_real_distribution<float> zJitterDistribution(-jitterZ, jitterZ);
	std::vector<Vector3D> candidates{};
	candidates.reserve(static_cast<std::size_t>(gridColumns * gridRows));
	std::vector<Vector3D> rejectedCandidates{};
	rejectedCandidates.reserve(static_cast<std::size_t>(gridColumns * gridRows));

	for (int row{ 0 }; row < gridRows; ++row)
	{
		for (int column{ 0 }; column < gridColumns; ++column)
		{
			const float x{ center_.x - halfExtents.x + (static_cast<float>(column) + 0.5f) * cellWidth + xJitterDistribution(randomEngine) };
			const float z{ center_.z - halfExtents.z + (static_cast<float>(row) + 0.5f) * cellDepth + zJitterDistribution(randomEngine) };
			candidates.emplace_back(x, center_.y, z);
		}
	}

	std::shuffle(candidates.begin(), candidates.end(), randomEngine);
	for (const Vector3D& candidate : candidates)
	{
		const Vector3D horizontalOffset{
			candidate.x - playerPosition_.x,
			0.0f,
			candidate.z - playerPosition_.z };

		if (horizontalOffset.GetSqrMagnitude() < minSqrDistance)
		{
			rejectedCandidates.push_back(candidate);
			continue;
		}

		spawnPositions.push_back(candidate);
		if (static_cast<int>(spawnPositions.size()) >= spawnCount_)
		{
			return spawnPositions;
		}
	}

	std::sort(rejectedCandidates.begin(), rejectedCandidates.end(), [&playerPosition_](const Vector3D& left_, const Vector3D& right_)
	{
		const Vector3D leftOffset{ left_.x - playerPosition_.x, 0.0f, left_.z - playerPosition_.z };
		const Vector3D rightOffset{ right_.x - playerPosition_.x, 0.0f, right_.z - playerPosition_.z };
		return leftOffset.GetSqrMagnitude() > rightOffset.GetSqrMagnitude();
	});

	for (const Vector3D& candidate : rejectedCandidates)
	{
		if (static_cast<int>(spawnPositions.size()) >= spawnCount_)
		{
			break;
		}

		spawnPositions.push_back(candidate);
	}

	return spawnPositions;
}
