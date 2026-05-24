#include "Precompiled.h"
#include "Scene_Test.h"

#include "Camera.h"
#include "MeshRenderer.h"
#include "PlayerController.h"
#include "Transform.h"

bool Scene_Test::LoadResources(ID3D12Device* device_)
{
	if (nullptr == device_)
	{
		return false;
	}

	testMesh = std::make_shared<Mesh>();
	if (!testMesh->LoadFromBinary(device_, "Resources/Models/Cube.bin"))
	{
		return false;
	}
	testMesh->SetId(1);

	testShader = std::make_shared<Shader>();
	if (!testShader->LoadFromFile(device_, L"Resources/Shaders/GameObject.hlsl"))
	{
		return false;
	}
	testShader->SetPipelineId(1);

	testMaterial = std::make_shared<Material>();
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
