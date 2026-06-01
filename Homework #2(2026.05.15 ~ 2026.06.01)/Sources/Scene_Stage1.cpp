#include "Precompiled.h"
#include "Scene_Stage1.h"

#include "Camera.h"
#include "CameraController.h"
#include "CubeCollider.h"
#include "EnemyController.h"
#include "GameObject.h"
#include "InputSystem.h"
#include "Logger.h"
#include "Light.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "PlayerController.h"
#include "ResourceSystem.h"
#include "Shader.h"
#include "StairCollider.h"
#include "StageSceneController.h"
#include "Transform.h"

void Scene_Stage1::OnLoad()
{
	const std::wstring_view StageMapPath{ L"Resources/Scenes/Scene_Stage1.bin" };
	BuildSceneObjects(StageMapPath);

	GameObject* const cameraObject{ Instantiate() };
	cameraObject->SetName(L"Main Camera");
	cameraObject->SetTag(L"MainCamera");
	
	Transform* const camTr{ cameraObject->GetComponent<Transform>() };
	camTr->SetWorldPosition(Vector3D{ 0.0f, 30.0f, -60.0f });
	camTr->SetLocalRotation(Quaternion::Euler(25.0f, 0.0f, 0.0f));
	
	Camera* const camera{ cameraObject->AddComponent<Camera>() };
	camera->SetClearMode(Camera::ClearType::SolidColor);
	camera->SetClearColor(ColorRGBA{ 1.0f, 0.9f, 0.05f, 1.0f });
	
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

void Scene_Stage1::OnUnload()
{
}

void Scene_Stage1::BuildSceneObjects(std::wstring_view mapPath_)
{
	std::ifstream file{ std::wstring(mapPath_.begin(), mapPath_.end()), std::ios::binary };
	if (file.is_open() == false)
	{
		return;
	}

	if (ReadTag(file, "<GameObjects>:") == false)
	{
		return;
	}

	uint32_t objectCount{ 0 };
	if (static_cast<bool>(file.read(reinterpret_cast<char*>(&objectCount), sizeof(uint32_t))) == false)
	{
		return;
	}

	Mesh* const defaultMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Cube.bin") };
	Material* const defaultMat{ ResourceSystem::GetInstance().GetResource<Material>(L"DefaultMaterial") };
	if (defaultMat != nullptr)
	{
		defaultMat->SetColor(ColorRGBA{ 1.0f, 1.0f, 1.0f, 1.0f });
	}

	for (uint32_t i{ 0 }; i < objectCount; ++i)
	{
		if (ReadTag(file, "<GameObject>:") == false)
		{
			break;
		}

		const std::wstring name{ ReadString(file) };
		
		Matrix4x4 worldMatrix{};
		if (static_cast<bool>(file.read(reinterpret_cast<char*>(&worldMatrix), sizeof(float) * 16)) == false)
		{
			break;
		}

		GameObject* const gameObject{ Instantiate() };
		gameObject->SetName(name);
		gameObject->GetComponent<Transform>()->SetWorldMatrix(worldMatrix);

		std::wstring meshPath{ L"Resources/Meshes/Cube.bin" };
		std::wstring matPath{ L"DefaultMaterial" };

		if (name.find(L"Player") != std::wstring::npos)
		{
			meshPath = L"Resources/Meshes/Player.bin";
			matPath = L"Resources/Materials/Player.bin";

			gameObject->SetTag(L"Player");
			gameObject->AddComponent<PlayerController>();
		}
		else if (name.find(L"Enemy") != std::wstring::npos)
		{
			meshPath = L"Resources/Meshes/Enemy.bin";
			matPath = L"Resources/Materials/Enemy.bin";

			gameObject->SetTag(L"Enemy");
			gameObject->AddComponent<EnemyController>();
		}
		else if (name.find(L"Wall") != std::wstring::npos)
		{
			meshPath = L"Resources/Meshes/Wall.bin";
			matPath = L"Resources/Materials/Wall_Day.bin";
		}
		else if (name.find(L"Stair") != std::wstring::npos)
		{
			meshPath = L"Resources/Meshes/Stair.bin";
			matPath = L"Resources/Materials/Stair_Day.bin";
		}
		else if (name.find(L"Floor") != std::wstring::npos)
		{
			meshPath = L"Resources/Meshes/Floor.bin";
			matPath = L"Resources/Materials/Floor_Day.bin";
		}

		Mesh* const mesh{ ResourceSystem::GetInstance().GetResource<Mesh>(meshPath) };
		Material* const mat{ ResourceSystem::GetInstance().GetResource<Material>(matPath) };

		MeshRenderer* const renderer{ gameObject->AddComponent<MeshRenderer>() };
		renderer->SetMesh(mesh != nullptr ? mesh : defaultMesh);
		renderer->SetMaterial(mat != nullptr ? mat : defaultMat);

		if (mesh != nullptr)
		{
			const Vector3D& min{ mesh->GetBoundsMin() };
			const Vector3D& max{ mesh->GetBoundsMax() };
			const Vector3D center{ (min + max) * 0.5f };
			const Vector3D size{ max - min };

			if (name.find(L"Stair") != std::wstring::npos)
			{
				GameObject* const physicsChild{ Instantiate() };
				physicsChild->SetName(name + L"_Physics");
				
				Transform* const childTransform{ physicsChild->GetComponent<Transform>() };
				childTransform->SetParent(gameObject->GetComponent<Transform>());
				
				childTransform->SetLocalRotation(Quaternion::Euler(45.0f, 0.0f, 0.0f));
				childTransform->SetLocalPosition(Vector3D::GetZero());
				childTransform->SetLocalScale(Vector3D::GetOne());

				CubeCollider* const collider{ physicsChild->AddComponent<CubeCollider>() };
				collider->SetCenter(center);
				collider->SetSize(size);
				collider->SetStatic(true);
				collider->UpdateVolume();
			}
			else
			{
				CubeCollider* const collider{ gameObject->AddComponent<CubeCollider>() };

				if (name.find(L"Player") != std::wstring::npos)
				{
					collider->SetCenter(Vector3D{ 0.0f, 0.0f, 0.0f });
					collider->SetSize(Vector3D{ 0.8f, 2.0f, 0.8f });
					collider->SetStatic(false);
				}
				else if (name.find(L"Enemy") != std::wstring::npos)
				{
					collider->SetCenter(center);
					collider->SetSize(size);
					collider->SetStatic(false);
				}
				else
				{
					collider->SetCenter(center);
					collider->SetSize(size);
					collider->SetStatic(true);
				}
				collider->UpdateVolume();
			}
		}

		const std::streampos pos{ file.tellg() };
		uint8_t nextTagLen{ 0 };
		if (static_cast<bool>(file.read(reinterpret_cast<char*>(&nextTagLen), sizeof(uint8_t))))
		{
			std::string nextTag(nextTagLen, '\0');
			file.read(&nextTag[0], nextTagLen);

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
}

bool Scene_Stage1::ReadTag(std::ifstream& file_, const std::string& expectedTag_)
{
	uint8_t tagLength{ 0 };
	if (static_cast<bool>(file_.read(reinterpret_cast<char*>(&tagLength), sizeof(uint8_t))) == false)
	{
		return false;
	}
	
	std::string tag(tagLength, '\0');
	if (static_cast<bool>(file_.read(&tag[0], tagLength)) == false)
	{
		return false;
	}

	return tag == expectedTag_;
}

std::wstring Scene_Stage1::ReadString(std::ifstream& file_)
{
	uint8_t strLength{ 0 };
	if (static_cast<bool>(file_.read(reinterpret_cast<char*>(&strLength), sizeof(uint8_t))) == false)
	{
		return L"";
	}
	
	std::string str(strLength, '\0');
	if (static_cast<bool>(file_.read(&str[0], strLength)) == false)
	{
		return L"";
	}

	return std::wstring(str.begin(), str.end());
}

