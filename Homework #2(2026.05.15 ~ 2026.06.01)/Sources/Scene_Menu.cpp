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
	// 카메라
	{
		GameObject* const cameraObject{ Instantiate() };
		cameraObject->SetName(L"Main Camera");
		cameraObject->SetTag(L"MainCamera");

		Transform* const cameraTransform{ cameraObject->GetComponent<Transform>() };
		cameraTransform->SetWorldPosition(Vector3D{ 0.0f, 0.0f, -20.0f });

		Camera* const camera{ cameraObject->AddComponent<Camera>() };
		camera->SetClearColor(ColorRGBA::GetBlue());

		MenuSceneController* menuController{ cameraObject->AddComponent<MenuSceneController>() };
		menuController->SetCamera(cameraObject->GetComponent<Camera>());
	}

	// 조명
	{
		GameObject* lightObject{ Instantiate() };
		lightObject->SetName(L"Main Light");
		lightObject->GetComponent<Transform>()->SetLocalRotation(Quaternion::Euler(45.0f, -45.0f, 0.0f));

		Light* light{ lightObject->AddComponent<Light>() };
		light->SetIntensity(1.0f);
		light->SetColor(ColorRGBA{ 1.0f, 1.0f, 1.0f, 1.0f });
	}

	// Stage Day 버튼
	{
		Material* dayMat{ ResourceSystem::GetInstance().GetResource<Material>(L"Resources/Materials/Floor_Day.bin") };
		Mesh* dayMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Day.bin") };

		GameObject* dayObject{ Instantiate() };
		dayObject->SetName(L"Day");
		dayObject->SetTag(L"MenuOption");
		dayObject->GetComponent<Transform>()->SetLocalPosition(Vector3D{ 0.0f, 10.0f, 20.0f });

		MeshRenderer* dayRenderer{ dayObject->AddComponent<MeshRenderer>() };
		dayRenderer->SetMesh(dayMesh);
		dayRenderer->SetMaterial(dayMat);

		if (dayMesh != nullptr)
		{
			CubeCollider* collider{ dayObject->AddComponent<CubeCollider>() };
			collider->SetCenter((dayMesh->GetBoundsMin() + dayMesh->GetBoundsMax()) * 0.5f);
			collider->SetSize(dayMesh->GetBoundsMax() - dayMesh->GetBoundsMin());
			collider->SetStatic(true);
			collider->UpdateVolume();
		}
	}

	// Stage Night 버튼
	{
		Material* nightMat{ ResourceSystem::GetInstance().GetResource<Material>(L"Resources/Materials/Floor_Night.bin") };
		Mesh* nightMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Night.bin") };

		GameObject* nightObject{ Instantiate() };
		nightObject->SetName(L"Night");
		nightObject->SetTag(L"MenuOption");
		nightObject->GetComponent<Transform>()->SetLocalPosition(Vector3D{ 0.0f, -10.0f, 20.0f });

		MeshRenderer* nightRenderer{ nightObject->AddComponent<MeshRenderer>() };
		nightRenderer->SetMesh(nightMesh);
		nightRenderer->SetMaterial(nightMat);

		if (nightMesh != nullptr)
		{
			CubeCollider* collider{ nightObject->AddComponent<CubeCollider>() };
			collider->SetCenter((nightMesh->GetBoundsMin() + nightMesh->GetBoundsMax()) * 0.5f);
			collider->SetSize(nightMesh->GetBoundsMax() - nightMesh->GetBoundsMin());
			collider->SetStatic(true);
			collider->UpdateVolume();
		}
	}
}

void Scene_Menu::OnUnload()
{
}
