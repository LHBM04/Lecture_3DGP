#include "Precompiled.h"

#include "Scene_Stage1.h"

#include "Camera.h"
#include "CameraController.h"
#include "CubeCollider.h"
#include "EnemyController.h"
#include "GameObject.h"
#include "Light.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "ResourceSystem.h"
#include "Transform.h"

void Scene_Stage1::OnLoad()
{
	constexpr std::wstring_view StageMapPath{ L"Resources/Scenes/Scene_Stage1.bin" };
	BuildSceneObjects(StageMapPath);
	SpawnEnemiesFromMap(StageMapPath);

	GameObject* cameraObject{ Instantiate() };
	cameraObject->SetName(L"Main Camera");
	cameraObject->SetTag(L"MainCamera");
	cameraObject->GetComponent<Transform>()->SetWorldPosition(Vector3D(0.0f, 30.0f, -60.0f));
	cameraObject->GetComponent<Transform>()->SetLocalRotation(Quaternion::Euler(25.0f, 0.0f, 0.0f));
	cameraObject->AddComponent<Camera>();
	
	GameObject* lightObject{ Instantiate() };
	lightObject->SetName(L"Main Light");
	lightObject->GetComponent<Transform>()->SetLocalRotation(Quaternion::Euler(45.0f, -45.0f, 0.0f));
	
	Light* light{ lightObject->AddComponent<Light>() };
	light->SetIntensity(1.5f);
	light->SetColor(ColorRGBA::GetWhite());

	CameraController* cameraController{ cameraObject->AddComponent<CameraController>() };
	cameraController->SetOffset(Vector3D{ 0.0f, 2.0f, -3.0f });

	GameObject* playerObject{ FindObjectWithTag(L"Player") };
	if (playerObject != nullptr)
	{
		cameraController->SetTarget(playerObject->GetComponent<Transform>());
	}
}

void Scene_Stage1::OnUnload()
{
	ResourceSystem::GetInstance().UnloadResource(L"Resources/Meshes/Cube.bin");
}

void Scene_Stage1::SpawnEnemiesFromMap(std::wstring_view mapPath_)
{
	std::ifstream file{ std::wstring(mapPath_.begin(), mapPath_.end()), std::ios::binary };
	if (!file.is_open())
	{
		return;
	}

	std::vector<char> bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	if (bytes.empty())
	{
		return;
	}

	const std::string enemyTag{ "<Enemies>:" };
	auto tagIt{ std::search(bytes.begin(), bytes.end(), enemyTag.begin(), enemyTag.end()) };
	if (tagIt == bytes.end())
	{
		return;
	}

	auto cursor = [&](std::size_t offset) -> const char*
	{
		return bytes.data() + offset;
	};

	const std::size_t tagOffset{ static_cast<std::size_t>(std::distance(bytes.begin(), tagIt)) };
	std::size_t readOffset{ tagOffset + enemyTag.size() };

	if (readOffset + sizeof(uint32_t) > bytes.size())
	{
		return;
	}

	uint32_t enemyCount{ 0 };
	std::memcpy(&enemyCount, cursor(readOffset), sizeof(uint32_t));
	readOffset += sizeof(uint32_t);

	for (uint32_t i{ 0 }; i < enemyCount; ++i)
	{
		if (readOffset + sizeof(float) * 3 > bytes.size())
		{
			break;
		}

		Vector3D position{};
		std::memcpy(&position.x, cursor(readOffset), sizeof(float));
		readOffset += sizeof(float);
		std::memcpy(&position.y, cursor(readOffset), sizeof(float));
		readOffset += sizeof(float);
		std::memcpy(&position.z, cursor(readOffset), sizeof(float));
		readOffset += sizeof(float);

		SpawnEnemyAt(position);
	}
}

void Scene_Stage1::SpawnEnemyAt(const Vector3D& position_)
{
	Mesh* enemyMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Enemy.bin") };
	Material* enemyMaterial{ ResourceSystem::GetInstance().GetResource<Material>(L"Resources/Materials/Enemy.bin") };
	Mesh* defaultMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Cube.bin") };
	Material* defaultMaterial{ ResourceSystem::GetInstance().GetResource<Material>(L"DefaultMaterial") };

	GameObject* enemy{ Instantiate() };
	enemy->SetName(L"Enemy");
	enemy->SetTag(L"Enemy");
	enemy->GetComponent<Transform>()->SetWorldPosition(position_);

	MeshRenderer* renderer{ enemy->AddComponent<MeshRenderer>() };
	renderer->SetMesh(enemyMesh != nullptr ? enemyMesh : defaultMesh);
	renderer->SetMaterial(enemyMaterial != nullptr ? enemyMaterial : defaultMaterial);

	if (enemyMesh != nullptr)
	{
		const Vector3D boundsMin{ enemyMesh->GetBoundsMin() };
		const Vector3D boundsMax{ enemyMesh->GetBoundsMax() };
		CubeCollider* collider{ enemy->AddComponent<CubeCollider>() };
		collider->SetCenter((boundsMin + boundsMax) * 0.5f);
		collider->SetSize(boundsMax - boundsMin);
		collider->SetStatic(false);
		collider->UpdateVolume();
	}

	enemy->AddComponent<EnemyController>();
}
