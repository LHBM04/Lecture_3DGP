#include "Precompiled.h"
#include "Scene_Title.h"

#include "AssetManager.h"
#include "GameObject.h"
#include "Mesh.h"
#include "MeshRenderer.h"

GameObject* cubeObject{ nullptr };

void Scene_Title::OnLoad()
{
	cube = AssetManager::Load<Mesh>(L"Assets/Cube.obj");
	if (nullptr != cube)
	{
		cubeObject = AddGameObject();
		cubeObject->SetName("Cube");

		MeshRenderer* const meshRenderer = cubeObject->AddComponent<MeshRenderer>();
		meshRenderer->SetMesh(cube);
	}
}

void Scene_Title::OnUpdate()
{
	auto transform = cubeObject->GetTransform();
	if (nullptr == transform)
	{
		return;
	}

	transform->SetWorldPosition(transform->GetWorldPosition() + Vector3D::GetUp() * 3.0f * 0.001f);
}

void Scene_Title::OnUnload()
{
	AssetManager::Unload(cube);
	cube = nullptr;
}
