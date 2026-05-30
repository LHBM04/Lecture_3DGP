#include "Precompiled.h"
#include "Scene_Title.h"

#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "ResourceSystem.h"
#include "Mesh.h"
#include "Material.h"
#include "Light.h"

void Scene_Title::OnLoad()
{
	// Camera
	GameObject* cameraObject{ Instantiate() };
	cameraObject->SetName(L"Main Camera");
	cameraObject->SetTag(L"MainCamera");

	Transform* cameraTransform{ cameraObject->GetComponent<Transform>() };
	cameraTransform->SetWorldPosition(Vector3D{ 0.0f, 2.0f, -5.0f });
	cameraTransform->SetLocalRotation(Quaternion::Euler(20.0f, 0.0f, 0.0f));

	cameraObject->AddComponent<Camera>();

	// Light
	GameObject* lightObject{ Instantiate() };
	lightObject->SetName(L"Main Light");

	Transform* lightTransform{ lightObject->GetComponent<Transform>() };
	lightTransform->SetLocalRotation(Quaternion::Euler(45.0f, -45.0f, 0.0f));

	Light* light{ lightObject->AddComponent<Light>() };
	light->SetIntensity(1.0f);
	light->SetColor(ColorRGBA{ 1.0f, 1.0f, 1.0f, 1.0f });

	// Cube
	Mesh* mesh{ ResourceSystem::GetInstance().GetOrLoadResource<Mesh>(L"Resources/Meshes/Cube.bin") };

	// Create default material manually since the file doesn't exist
	Material* mat{ ResourceSystem::GetInstance().GetOrLoadResource<Material>(L"DefaultMaterial") };
	if (mat != nullptr)
	{
		mat->SetBaseColor(Vector4D{ 1.0f, 1.0f, 1.0f, 1.0f });
	}

	GameObject* cubeObject{ Instantiate() };
	cubeObject->SetName(L"Cube");

	Transform* cubeTransform{ cubeObject->GetComponent<Transform>() };
	cubeTransform->SetLocalPosition(Vector3D{ 0.0f, 0.0f, 0.0f });
	cubeTransform->SetLocalRotation(Quaternion::Euler(0.0f, 45.0f, 0.0f));
	cubeTransform->SetLocalScale(Vector3D{ 2.0f, 2.0f, 2.0f });

	MeshRenderer* meshRenderer{ cubeObject->AddComponent<MeshRenderer>() };
	meshRenderer->SetMesh(mesh);
	meshRenderer->SetMaterial(mat);
	}

void Scene_Title::OnUnload()
{
	ResourceSystem::GetInstance().UnloadResource(L"Resources/Meshes/Cube.bin");
	ResourceSystem::GetInstance().UnloadResource(L"DefaultMaterial");
}
