#include "Precompiled.h"

#include "Scene_Menu.h"

#include "Camera.h"
#include "CubeCollider.h"
#include "GameObject.h"
#include "Light.h"
#include "Material.h"
#include "MenuSceneController.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "ResourceSystem.h"
#include "Transform.h"

void Scene_Menu::OnLoad()
{
	GameObject* cameraObject{ Instantiate() };
	cameraObject->SetName(L"Main Camera");
	cameraObject->SetTag(L"MainCamera");
	cameraObject->GetComponent<Transform>()->SetWorldPosition(Vector3D{ 0.0f, 0.0f, -20.0f });
	cameraObject->AddComponent<Camera>();

	GameObject* lightObject{ Instantiate() };
	lightObject->SetName(L"Main Light");
	lightObject->GetComponent<Transform>()->SetLocalRotation(Quaternion::Euler(45.0f, -45.0f, 0.0f));

	Light* light{ lightObject->AddComponent<Light>() };
	light->SetIntensity(1.0f);
	light->SetColor(ColorRGBA{ 1.0f, 1.0f, 1.0f, 1.0f });

	Material* defaultMat{ ResourceSystem::GetInstance().GetResource<Material>(L"DefaultMaterial") };
	Material* dayMat{ ResourceSystem::GetInstance().GetResource<Material>(L"Resources/Materials/Floor_Day.bin") };
	Material* nightMat{ ResourceSystem::GetInstance().GetResource<Material>(L"Resources/Materials/Floor_Night.bin") };
	Mesh* dayMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Day.bin") };
	Mesh* nightMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Night.bin") };

	GameObject* dayObject{ Instantiate() };
	dayObject->SetName(L"Day");
	dayObject->SetTag(L"MenuOption");
	dayObject->GetComponent<Transform>()->SetLocalPosition(Vector3D{ 0.0f, 8.0f, 75.0f });
	dayObject->GetComponent<Transform>()->SetLocalScale(Vector3D{ 3.0f, 3.0f, 3.0f });
	MeshRenderer* dayRenderer{ dayObject->AddComponent<MeshRenderer>() };
	dayRenderer->SetMesh(dayMesh);
	dayRenderer->SetMaterial(dayMat != nullptr ? dayMat : defaultMat);

	if (dayMesh != nullptr)
	{
		CubeCollider* collider{ dayObject->AddComponent<CubeCollider>() };
		collider->SetCenter((dayMesh->GetBoundsMin() + dayMesh->GetBoundsMax()) * 0.5f);
		collider->SetSize(dayMesh->GetBoundsMax() - dayMesh->GetBoundsMin());
		collider->SetStatic(true);
		collider->UpdateVolume();
	}

	GameObject* nightObject{ Instantiate() };
	nightObject->SetName(L"Night");
	nightObject->SetTag(L"MenuOption");
	nightObject->GetComponent<Transform>()->SetLocalPosition(Vector3D{ 0.0f, -8.0f, 75.0f });
	nightObject->GetComponent<Transform>()->SetLocalScale(Vector3D{ 3.0f, 3.0f, 3.0f });
	MeshRenderer* nightRenderer{ nightObject->AddComponent<MeshRenderer>() };
	nightRenderer->SetMesh(nightMesh);
	nightRenderer->SetMaterial(nightMat != nullptr ? nightMat : defaultMat);

	if (nightMesh != nullptr)
	{
		CubeCollider* collider{ nightObject->AddComponent<CubeCollider>() };
		collider->SetCenter((nightMesh->GetBoundsMin() + nightMesh->GetBoundsMax()) * 0.5f);
		collider->SetSize(nightMesh->GetBoundsMax() - nightMesh->GetBoundsMin());
		collider->SetStatic(true);
		collider->UpdateVolume();
	}

	MenuSceneController* menuController{ cameraObject->AddComponent<MenuSceneController>() };
	menuController->SetCamera(cameraObject->GetComponent<Camera>());
}

void Scene_Menu::OnUnload()
{
}
