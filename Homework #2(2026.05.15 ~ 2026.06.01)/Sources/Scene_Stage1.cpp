#include "Precompiled.h"

#include "Scene_Stage1.h"

#include "Camera.h"
#include "CameraController.h"
#include "CubeCollider.h"
#include "EnemyController.h"
#include "GameObject.h"
#include "Logger.h"
#include "Light.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "PlayerController.h"
#include "ResourceSystem.h"
#include "Shader.h"
#include "StairCollider.h"
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
	Camera* camera{ cameraObject->AddComponent<Camera>() };
	camera->SetClearMode(Camera::ClearType::SolidColor);
	camera->SetClearColor(ColorRGBA{ 0.53f, 0.81f, 0.92f, 1.0f });
	
	GameObject* lightObject{ Instantiate() };
	lightObject->SetName(L"Main Light");
	lightObject->GetComponent<Transform>()->SetLocalRotation(Quaternion::Euler(45.0f, -45.0f, 0.0f));
	
	Light* light{ lightObject->AddComponent<Light>() };
	light->SetIntensity(1.5f);
	light->SetColor(ColorRGBA::GetWhite());

	CameraController* cameraController{ cameraObject->AddComponent<CameraController>() };
	cameraController->SetThirdPersonOffset(Vector3D{ 0.0f, 2.0f, -3.0f });
	cameraController->SetFirstPersonOffset(Vector3D{ 0.0f, 1.6f, 0.0f });

	GameObject* playerObject{ FindObjectWithTag(L"Player") };
	if (playerObject != nullptr)
	{
		cameraController->SetTarget(playerObject->GetComponent<Transform>());
	}

	GameObject* crosshairObject{ Instantiate() };
	crosshairObject->SetName(L"Crosshair");
	crosshairObject->SetTag(L"UI");

	Mesh* crosshairMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Crosshair.bin") };
	Material* crosshairMaterial{ ResourceSystem::GetInstance().GetResource<Material>(L"Resources/Materials/Crosshair.bin") };
	Shader* uiShader{ ResourceSystem::GetInstance().GetResource<Shader>(L"Resources/Shaders/UIObject.hlsl") };
	if (crosshairMaterial != nullptr && uiShader != nullptr)
	{
		crosshairMaterial->SetShader(uiShader);
	}

	Transform* crosshairTransform{ crosshairObject->GetComponent<Transform>() };
	crosshairTransform->SetParent(cameraObject->GetComponent<Transform>());
	crosshairTransform->SetLocalPosition(Vector3D(0.0f, 0.0f, 0.2f));
	crosshairTransform->SetLocalRotation(Quaternion::GetIdentity());
	crosshairTransform->SetLocalScale(Vector3D(0.03f, 0.03f, 0.03f));

	MeshRenderer* crosshairRenderer{ crosshairObject->AddComponent<MeshRenderer>() };
	crosshairRenderer->SetMesh(crosshairMesh);
	crosshairRenderer->SetMaterial(crosshairMaterial);
	crosshairObject->SetActive(false);

	cameraController->SetCrosshairObject(crosshairObject);
}

void Scene_Stage1::OnUnload()
{
	ResourceSystem::GetInstance().UnloadResource(L"Resources/Meshes/Cube.bin");
}

void Scene_Stage1::BuildSceneObjects(std::wstring_view mapPath_)
{
	std::ifstream file{ std::wstring(mapPath_.begin(), mapPath_.end()), std::ios::binary };
	if (!file.is_open())
	{
		return;
	}

	if (!ReadTag(file, "<GameObjects>:")) return;

	uint32_t objectCount{ 0 };
	if (!static_cast<bool>(file.read(reinterpret_cast<char*>(&objectCount), sizeof(uint32_t)))) return;

	if (objectCount == 0)
	{
		Logger::Warning(L"[StageLoad] objectCount is 0. map='{}'", std::wstring(mapPath_));
		return;
	}

	const std::streamoff totalBytes{ static_cast<std::streamoff>(std::filesystem::file_size(std::wstring(mapPath_.begin(), mapPath_.end()))) };
	const std::streamoff currentPos{ file.tellg() };
	const std::streamoff remainBytes{ std::max<std::streamoff>(0, totalBytes - currentPos) };
	constexpr std::streamoff MinBytesPerObject{ 1 + 12 + 1 + 1 + (sizeof(float) * 16) };
	const uint32_t maxPossibleObjects{ static_cast<uint32_t>(remainBytes / MinBytesPerObject) };
	if (maxPossibleObjects == 0)
	{
		Logger::Error(L"[StageLoad] maxPossibleObjects is 0. map='{}'", std::wstring(mapPath_));
		return;
	}
	if (objectCount > maxPossibleObjects)
	{
		Logger::Warning(L"[StageLoad] objectCount clamped {} -> {}. map='{}'", objectCount, maxPossibleObjects, std::wstring(mapPath_));
		objectCount = maxPossibleObjects;
	}
	Logger::Trace(L"[StageLoad] Begin map='{}' objectCount={}", std::wstring(mapPath_), objectCount);

	Mesh* defaultMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Cube.bin") };
	Material* defaultMat{ ResourceSystem::GetInstance().GetResource<Material>(L"DefaultMaterial") };
	if (defaultMat != nullptr)
	{
		defaultMat->SetBaseColor(Vector4D(1.0f, 1.0f, 1.0f, 1.0f));
	}

	for (uint32_t i{ 0 }; i < objectCount; ++i)
	{
		if (!file.good())
		{
			break;
		}

		if (!ReadTag(file, "<GameObject>:")) break;

		std::wstring name{ ReadString(file) };

		Matrix4x4 worldMatrix;
		if (!static_cast<bool>(file.read(reinterpret_cast<char*>(&worldMatrix), sizeof(float) * 16))) break;

		GameObject* go{ Instantiate() };
		go->SetName(name);
		go->GetComponent<Transform>()->SetWorldMatrix(worldMatrix);

		std::wstring meshPath{ L"Resources/Meshes/Cube.bin" };
		std::wstring matPath{ L"DefaultMaterial" };

		if (name.find(L"Player") != std::wstring::npos)
		{
			meshPath = L"Resources/Meshes/Player.bin";
			matPath = L"Resources/Materials/Player.bin";
			go->SetTag(L"Player");
			go->AddComponent<PlayerController>();
		}
		else if (name.find(L"Enemy") != std::wstring::npos)
		{
			meshPath = L"Resources/Meshes/Enemy.bin";
			matPath = L"Resources/Materials/Enemy.bin";
			go->SetTag(L"Enemy");
			go->AddComponent<EnemyController>();
		}
		else if (name.find(L"Wall") != std::wstring::npos)
		{
			meshPath = L"Resources/Meshes/Wall.bin";
			matPath = L"Resources/Materials/Wall_Day.bin";
		}
		else if (name.find(L"Stair") != std::wstring::npos)
		{
			meshPath = L"Resources/Meshes/Stairs.bin";
			matPath = L"Resources/Materials/Stairs_Day.bin";
		}
		else if (name.find(L"Floor") != std::wstring::npos)
		{
			meshPath = L"Resources/Meshes/Floor.bin";
			matPath = L"Resources/Materials/Floor_Day.bin";
		}

		Mesh* mesh{ ResourceSystem::GetInstance().GetResource<Mesh>(meshPath) };
		Material* mat{ ResourceSystem::GetInstance().GetResource<Material>(matPath) };
		const Vector3D worldPos{ worldMatrix.GetWorldPosition() };

		if (i < 200 || (i % 500) == 0)
		{
			Logger::Trace(L"[StageLoad] [{}] name='{}' pos=({:.2f},{:.2f},{:.2f}) mesh='{}' mat='{}'",
				i, name, worldPos.x, worldPos.y, worldPos.z, meshPath, matPath);
		}

		MeshRenderer* renderer{ go->AddComponent<MeshRenderer>() };
		renderer->SetMesh(mesh != nullptr ? mesh : defaultMesh);
		renderer->SetMaterial(mat != nullptr ? mat : defaultMat);

		if (mesh != nullptr)
		{
			const Vector3D& min{ mesh->GetBoundsMin() };
			const Vector3D& max{ mesh->GetBoundsMax() };
			Vector3D center{ (min + max) * 0.5f };
			Vector3D size{ max - min };

			if (name.find(L"Stair") != std::wstring::npos)
			{
				StairCollider* stairCollider{ go->AddComponent<StairCollider>() };
				stairCollider->SetCenter(center);
				stairCollider->SetSize(size);
				stairCollider->SetSlopeAxis(StairCollider::SlopeAxis::PositiveZ);
				stairCollider->SetStatic(true);
				stairCollider->UpdateVolume();
			}
			else
			{
				CubeCollider* collider{ go->AddComponent<CubeCollider>() };
				collider->SetCenter(center);
				collider->SetSize(size);
				collider->UpdateVolume();
				collider->SetStatic(name.find(L"Player") == std::wstring::npos && name.find(L"Enemy") == std::wstring::npos);
			}
		}

		std::streampos pos{ file.tellg() };
		uint8_t nextTagLen{ 0 };
		if (static_cast<bool>(file.read(reinterpret_cast<char*>(&nextTagLen), sizeof(uint8_t))))
		{
			std::string nextTag(nextTagLen, '\0');
			if (!static_cast<bool>(file.read(&nextTag[0], nextTagLen)))
			{
				break;
			}

			if (nextTag != "<Mesh>:")
			{
				file.seekg(pos);
			}
			else
			{
				ReadString(file);
			}
		}
	}
	Logger::Info(L"[StageLoad] End map='{}' processed={}", std::wstring(mapPath_), objectCount);
}

bool Scene_Stage1::ReadTag(std::ifstream& file_, const std::string& expectedTag_)
{
	uint8_t tagLength{ 0 };
	if (!static_cast<bool>(file_.read(reinterpret_cast<char*>(&tagLength), sizeof(uint8_t)))) return false;

	std::string tag(tagLength, '\0');
	if (!static_cast<bool>(file_.read(&tag[0], tagLength))) return false;

	return tag == expectedTag_;
}

std::wstring Scene_Stage1::ReadString(std::ifstream& file_)
{
	uint8_t strLength{ 0 };
	if (!static_cast<bool>(file_.read(reinterpret_cast<char*>(&strLength), sizeof(uint8_t)))) return L"";

	std::string str(strLength, '\0');
	if (!static_cast<bool>(file_.read(&str[0], strLength))) return L"";

	return std::wstring(str.begin(), str.end());
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

	const std::size_t remainBytes{ bytes.size() - readOffset };
	const uint32_t maxPossibleByBytes{ static_cast<uint32_t>(remainBytes / (sizeof(float) * 3)) };
	if (enemyCount > maxPossibleByBytes)
	{
		enemyCount = maxPossibleByBytes;
	}
	constexpr uint32_t RuntimeEnemyCap{ 256 };
	if (enemyCount > RuntimeEnemyCap)
	{
		enemyCount = RuntimeEnemyCap;
	}

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
