#include "Precompiled.h"
#include "Scene_Test.h"

#include "Camera.h"
#include "MeshRenderer.h"
#include "PlayerController.h"
#include "ResourceManager.h"
#include "Transform.h"

bool Scene_Test::LoadResources(ID3D12Device* device_)
{
	if (nullptr == device_)
	{
		return false;
	}

	ResourceManager& rm{ ResourceManager::GetInstance() };

	testMesh = rm.Create<Mesh>("Stairs");
	if (!testMesh->LoadFromBinary(device_, "Resources/Models/Stairs.bin"))
	{
		return false;
	}
	testMesh->SetId(1);

	testShader = rm.Create<Shader>("GameObjectShader");
	if (!testShader->LoadFromFile(device_, L"Resources/Shaders/GameObject.hlsl"))
	{
		return false;
	}
	testShader->SetPipelineId(1);

	testMaterial = rm.Create<Material>("TestMaterial");
	testMaterial->SetId(1);
	testMaterial->SetColor(ColorRGBA::GetWhite());
	testMaterial->SetShader(testShader.get());

	return true;
}

void Scene_Test::OnLoad()
{
	GameObject& cameraObject{ CreateGameObject("Main Camera") };
	Transform* cameraTransform{ cameraObject.AddComponent<Transform>() };
	Camera* camera{ cameraObject.AddComponent<Camera>() };

	cameraTransform->SetLocalPosition(Vector3D(0.0f, 0.0f, -10.0f));
	camera->SetProjection(Camera::Projection::Persprective);

	GameObject& meshObject{ CreateGameObject("Test Mesh") };
	meshObject.AddComponent<Transform>();

	MeshRenderer* meshRenderer{ meshObject.AddComponent<MeshRenderer>() };
	meshRenderer->SetMesh(testMesh.get());
	meshRenderer->SetMaterial(testMaterial.get());

	PlayerController* playerController{ meshObject.AddComponent<PlayerController>() };
	playerController->SetRotationSpeed(1.5f);
}
