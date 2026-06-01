#include "Precompiled.h"
#include "Scene_Stage2.h"

#include "Camera.h"
#include "CameraController.h"
#include "CubeCollider.h"
#include "EnemyController.h"
#include "GameObject.h"
#include "Light.h"
#include "Logger.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "PlayerController.h"
#include "ResourceSystem.h"
#include "Shader.h"
#include "StairCollider.h"
#include "StageSceneController.h"
#include "Transform.h"

void Scene_Stage2::OnLoad()
{
	const std::wstring_view StageMapPath{ L"Resources/Scenes/Scene_Stage2.bin" };
	BuildSceneObjects(StageMapPath);

	GameObject* const cameraObject{ Instantiate() };
	cameraObject->SetName(L"Main Camera");
	cameraObject->SetTag(L"MainCamera");

	Transform* const camTr{ cameraObject->GetComponent<Transform>() };
	camTr->SetWorldPosition(Vector3D{ 0.0f, 30.0f, -60.0f });
	camTr->SetLocalRotation(Quaternion::Euler(25.0f, 0.0f, 0.0f));

	Camera* const camera{ cameraObject->AddComponent<Camera>() };
	camera->SetClearMode(Camera::ClearType::SolidColor);
	camera->SetClearColor(ColorRGBA{ 0.02f, 0.05f, 0.25f, 1.0f });

	GameObject* const lightObject{ Instantiate() };
	lightObject->SetName(L"Main Light");
	lightObject->GetComponent<Transform>()->SetLocalRotation(Quaternion::Euler(45.0f, -45.0f, 0.0f));

	Light* const light{ lightObject->AddComponent<Light>() };
	light->SetIntensity(1.5f);
	light->SetColor(ColorRGBA::GetWhite());

	CameraController* const cameraController{ cameraObject->AddComponent<CameraController>() };
	cameraController->SetThirdPersonOffset(Vector3D{ 0.0f, 2.0f, -3.0f });
	cameraController->SetFirstPersonOffset(Vector3D{ 0.0f, 1.6f, 0.0f });

	GameObject* const playerObject{ FindObjectWithTag(L"Player") };
	if (playerObject != nullptr)
	{
		cameraController->SetTarget(playerObject->GetComponent<Transform>());
	}

	GameObject* const crosshairObject{ Instantiate() };
	crosshairObject->SetName(L"Crosshair");
	crosshairObject->SetTag(L"UI");

	Mesh* const crosshairMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Crosshair.bin") };
	Material* const crosshairMaterial{ ResourceSystem::GetInstance().GetResource<Material>(L"Resources/Materials/Crosshair.bin") };
	Shader* const uiShader{ ResourceSystem::GetInstance().GetResource<Shader>(L"Resources/Shaders/UIObject.hlsl") };
	if (crosshairMaterial != nullptr && uiShader != nullptr)
	{
		crosshairMaterial->SetShader(uiShader);
	}

	Transform* const crosshairTransform{ crosshairObject->GetComponent<Transform>() };
	crosshairTransform->SetParent(cameraObject->GetComponent<Transform>());
	crosshairTransform->SetLocalPosition(Vector3D{ 0.0f, 0.0f, 0.2f });
	crosshairTransform->SetLocalRotation(Quaternion::GetIdentity());
	crosshairTransform->SetLocalScale(Vector3D{ 0.03f, 0.03f, 0.03f });

	MeshRenderer* const crosshairRenderer{ crosshairObject->AddComponent<MeshRenderer>() };
	crosshairRenderer->SetMesh(crosshairMesh);
	crosshairRenderer->SetMaterial(crosshairMaterial);
	crosshairObject->SetActive(false);

	cameraController->SetCrosshairObject(crosshairObject);
	cameraObject->AddComponent<StageSceneController>();
}

void Scene_Stage2::OnUnload()
{
}

void Scene_Stage2::BuildSceneObjects(std::wstring_view mapPath_)
{
	std::ifstream file{ std::wstring(mapPath_.begin(), mapPath_.end()), std::ios::binary };
	if (!file.is_open())
	{
		return;
	}

	if (!ReadTag(file, "<GameObjects>:"))
	{
		return;
	}

	uint32_t objectCount{ 0 };
	if (!static_cast<bool>(file.read(reinterpret_cast<char*>(&objectCount), sizeof(uint32_t))))
	{
		return;
	}

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

	for (uint32_t i{ 0 }; i < objectCount; ++i)
	{
		if (!file.good())
		{
			break;
		}

		if (!ReadTag(file, "<GameObject>:"))
		{
			break;
		}

		std::wstring name{ ReadString(file) };

		Matrix4x4 worldMatrix;
		if (!static_cast<bool>(file.read(reinterpret_cast<char*>(&worldMatrix), sizeof(float) * 16)))
		{
			break;
		}

		GameObject* gameObject{ Instantiate() };
		gameObject->SetName(name);
		gameObject->GetComponent<Transform>()->SetWorldMatrix(worldMatrix);

		std::wstring meshPath{ L"Resources/Meshes/Cube.bin" };
		std::wstring matPath{ L"DefaultMaterial" };
		bool isStaticCollider{ true };

		if (name.find(L"Player") != std::wstring::npos)
		{
			meshPath = L"Resources/Meshes/Player.bin";
			matPath = L"Resources/Materials/Player.bin";
			gameObject->SetTag(L"Player");
			gameObject->AddComponent<PlayerController>();
			isStaticCollider = false;
		}
		else if (name.find(L"Enemy") != std::wstring::npos)
		{
			meshPath = L"Resources/Meshes/Enemy.bin";
			matPath = L"Resources/Materials/Enemy.bin";
			gameObject->SetTag(L"Enemy");
			gameObject->AddComponent<EnemyController>();
			isStaticCollider = false;
		}
		else if (name.find(L"Wall") != std::wstring::npos)
		{
			meshPath = L"Resources/Meshes/Wall.bin";
			matPath = L"Resources/Materials/Wall_Night.bin";
		}
		else if (name.find(L"Stair") != std::wstring::npos)
		{
			meshPath = L"Resources/Meshes/Stairs.bin";
			matPath = L"Resources/Materials/Stairs_Night.bin";
		}
		else if (name.find(L"Floor") != std::wstring::npos)
		{
			meshPath = L"Resources/Meshes/Floor.bin";
			matPath = L"Resources/Materials/Floor_Night.bin";
		}

		Mesh* mesh{ ResourceSystem::GetInstance().GetResource<Mesh>(meshPath) };
		Material* mat{ ResourceSystem::GetInstance().GetResource<Material>(matPath) };
		const Vector3D worldPos{ worldMatrix.GetWorldPosition() };

		if (i < 200 || (i % 500) == 0)
		{
			Logger::Trace(L"[StageLoad] [{}] name='{}' pos=({:.2f},{:.2f},{:.2f}) mesh='{}' mat='{}'",
				i, name, worldPos.x, worldPos.y, worldPos.z, meshPath, matPath);
		}

		MeshRenderer* renderer{ gameObject->AddComponent<MeshRenderer>() };
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
				StairCollider* stairCollider{ gameObject->AddComponent<StairCollider>() };
				stairCollider->SetCenter(center);
				stairCollider->SetSize(size);
				stairCollider->SetSlopeAxis(StairCollider::SlopeAxis::PositiveZ);
				stairCollider->SetStatic(true);
				stairCollider->UpdateVolume();
			}
			else
			{
				CubeCollider* collider{ gameObject->AddComponent<CubeCollider>() };
				if (name.find(L"Player") != std::wstring::npos)
				{
					collider->SetCenter(Vector3D{ 0.0f, 0.0f, 0.0f });
					collider->SetSize(Vector3D{ 0.8f, 2.0f, 0.8f });
					collider->SetStatic(false);
				}
				else
				{
					collider->SetCenter(center);
					collider->SetSize(size);
					collider->SetStatic(isStaticCollider);
				}
				collider->UpdateVolume();
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

bool Scene_Stage2::ReadTag(std::ifstream& file_, const std::string& expectedTag_)
{
	uint8_t tagLength{ 0 };
	if (!static_cast<bool>(file_.read(reinterpret_cast<char*>(&tagLength), sizeof(uint8_t))))
	{
		return false;
	}

	std::string tag(tagLength, '\0');
	if (!static_cast<bool>(file_.read(&tag[0], tagLength)))
	{
		return false;
	}

	return tag == expectedTag_;
}

std::wstring Scene_Stage2::ReadString(std::ifstream& file_)
{
	uint8_t strLength{ 0 };
	if (!static_cast<bool>(file_.read(reinterpret_cast<char*>(&strLength), sizeof(uint8_t))))
	{
		return L"";
	}

	std::string str(strLength, '\0');
	if (!static_cast<bool>(file_.read(&str[0], strLength)))
	{
		return L"";
	}

	return std::wstring(str.begin(), str.end());
}
