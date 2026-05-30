#include "Precompiled.h"
#include "Scene_Stage1.h"

#include "Camera.h"
#include "CameraController.h"
#include "GameObject.h"
#include "Light.h"
#include "ResourceSystem.h"
#include "Transform.h"

void Scene_Stage1::OnLoad()
{
	BuildSceneObjects(L"Resources/Scenes/Scene.bin");

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
