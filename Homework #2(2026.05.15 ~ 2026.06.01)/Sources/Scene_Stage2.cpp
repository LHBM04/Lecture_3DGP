#include "Precompiled.h"
#include "Scene_Stage2.h"

#include "CubeCollider.h"
#include "EnemyController.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "PlayerController.h"
#include "ResourceSystem.h"
#include "StairCollider.h"
#include "Transform.h"

void Scene_Stage2::OnLoad()
{
	BuildSceneObjects(L"Resources/Scenes/Scene_Stage1.bin");
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

	Mesh* defaultMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Cube.bin") };
	Material* defaultMat{ ResourceSystem::GetInstance().GetResource<Material>(L"DefaultMaterial") };

	for (uint32_t i{ 0 }; i < objectCount; ++i)
	{
		if (!ReadTag(file, "<GameObject>:"))
		{
			break;
		}

		const std::wstring name{ ReadString(file) };

		Matrix4x4 worldMatrix;
		if (!static_cast<bool>(file.read(reinterpret_cast<char*>(&worldMatrix), sizeof(float) * 16)))
		{
			break;
		}

		GameObject* go{ Instantiate() };
		go->SetName(name);
		go->GetComponent<Transform>()->SetWorldMatrix(worldMatrix);

		std::wstring meshPath{ L"Resources/Meshes/Cube.bin" };
		std::wstring matPath{ L"DefaultMaterial" };
		bool isStaticCollider{ true };

		if (name.find(L"Player") != std::wstring::npos)
		{
			meshPath = L"Resources/Meshes/Player.bin";
			matPath = L"Resources/Materials/Player.bin";
			go->SetTag(L"Player");
			go->AddComponent<PlayerController>();
			isStaticCollider = false;
		}
		else if (name.find(L"Enemy") != std::wstring::npos)
		{
			meshPath = L"Resources/Meshes/Enemy.bin";
			matPath = L"Resources/Materials/Enemy.bin";
			go->SetTag(L"Enemy");
			go->AddComponent<EnemyController>();
			isStaticCollider = false;
		}
		else if (name.find(L"Wall") != std::wstring::npos)
		{
			meshPath = L"Resources/Meshes/Wall.bin";
			matPath = L"Resources/Materials/Wall.bin";
		}
		else if (name.find(L"Stair") != std::wstring::npos)
		{
			meshPath = L"Resources/Meshes/Stairs.bin";
			matPath = L"Resources/Materials/Stairs.bin";
		}
		else if (name.find(L"Floor") != std::wstring::npos)
		{
			meshPath = L"Resources/Meshes/Floor.bin";
			matPath = L"Resources/Materials/Floor.bin";
		}

		Mesh* mesh{ ResourceSystem::GetInstance().GetResource<Mesh>(meshPath) };
		Material* mat{ ResourceSystem::GetInstance().GetResource<Material>(matPath) };

		MeshRenderer* renderer{ go->AddComponent<MeshRenderer>() };
		renderer->SetMesh(mesh != nullptr ? mesh : defaultMesh);
		renderer->SetMaterial(mat != nullptr ? mat : defaultMat);

		if (mesh != nullptr)
		{
			const Vector3D& min{ mesh->GetBoundsMin() };
			const Vector3D& max{ mesh->GetBoundsMax() };
			if (name.find(L"Stair") != std::wstring::npos)
			{
				StairCollider* stairCollider{ go->AddComponent<StairCollider>() };
				stairCollider->SetCenter((min + max) * 0.5f);
				stairCollider->SetSize(max - min);
				stairCollider->SetSlopeAxis(StairCollider::SlopeAxis::PositiveZ);
				stairCollider->SetStatic(true);
				stairCollider->UpdateVolume();
			}
			else
			{
				CubeCollider* collider{ go->AddComponent<CubeCollider>() };
				collider->SetCenter((min + max) * 0.5f);
				collider->SetSize(max - min);
				collider->SetStatic(isStaticCollider);
				collider->UpdateVolume();
			}
		}
	}
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
