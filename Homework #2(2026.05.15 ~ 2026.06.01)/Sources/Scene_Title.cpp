#include "Precompiled.h"

#include "Scene_Title.h"

#include "Camera.h"
#include "CubeCollider.h"
#include "GameObject.h"
#include "Light.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "ResourceSystem.h"
#include "TitleSceneController.h"
#include "Transform.h"

void Scene_Title::OnLoad()
{
	GameObject* const cameraObject{ Instantiate() };
	cameraObject->SetName(L"Main Camera");
	cameraObject->SetTag(L"MainCamera");
	cameraObject->GetComponent<Transform>()->SetWorldPosition(Vector3D{ 0.0f, 0.0f, -10.0f });

	Camera* const camera{ cameraObject->AddComponent<Camera>() };
	camera->SetClearMode(Camera::ClearType::SolidColor);
	camera->SetClearColor(ColorRGBA::GetBlack());

	GameObject* const lightObject{ Instantiate() };
	lightObject->SetName(L"Main Light");
	lightObject->GetComponent<Transform>()->SetLocalRotation(Quaternion::Euler(45.0f, -45.0f, 0.0f));

	Light* const light{ lightObject->AddComponent<Light>() };
	light->SetIntensity(1.0f);
	light->SetColor(ColorRGBA::GetWhite());
	
	// 타이틀 로고
	{
		GameObject* const titleObject{ Instantiate() };
		titleObject->SetName(L"Title Logo");
		titleObject->SetTag(L"UI");

		Transform* const titleTransform{ titleObject->GetComponent<Transform>() };
		titleTransform->SetLocalPosition(Vector3D{ 0.0f, 10.0f, 50.0f });

		Mesh* const titleMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Title.bin") };
		Material* const titleMaterial{ ResourceSystem::GetInstance().GetResource<Material>(L"DefaultMaterial") };
		if (titleMaterial != nullptr)
		{
			titleMaterial->SetColor(ColorRGBA::GetWhite().ToVector4D());
		}

		MeshRenderer* const renderer{ titleObject->AddComponent<MeshRenderer>() };
		renderer->SetMesh(titleMesh);
		renderer->SetMaterial(titleMaterial);
	}

	// 시작 버튼.
	{
		GameObject* const buttonObject{ Instantiate() };
		buttonObject->SetName(L"Start Button");
		buttonObject->SetTag(L"Button");

		Transform* const buttonTransform{ buttonObject->GetComponent<Transform>() };
		buttonTransform->SetLocalPosition(Vector3D{ 0.0f, -10.0f, 50.0f });

		Mesh* const buttonMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Button.bin") };
		Material* const buttonMaterial{ ResourceSystem::GetInstance().GetResource<Material>(L"DefaultMaterial") };
		if (buttonMaterial != nullptr)
		{
			buttonMaterial->SetColor(ColorRGBA::GetWhite().ToVector4D());
		}

		MeshRenderer* const renderer{ buttonObject->AddComponent<MeshRenderer>() };
		renderer->SetMesh(buttonMesh);
		renderer->SetMaterial(buttonMaterial);

		CubeCollider* const buttonCollider{ buttonObject->AddComponent<CubeCollider>() };
		if (buttonMesh != nullptr)
		{
			const Vector3D boundsMin{ buttonMesh->GetBoundsMin() };
			const Vector3D boundsMax{ buttonMesh->GetBoundsMax() };
			buttonCollider->SetCenter((boundsMin + boundsMax) * 0.5f);
			buttonCollider->SetSize(boundsMax - boundsMin);
		}
		else
		{
			buttonCollider->SetCenter(Vector3D::GetZero());
			buttonCollider->SetSize(Vector3D{ 4.0f, 2.0f, 0.5f });
		}
		buttonCollider->SetStatic(true);
		buttonCollider->UpdateVolume();
	}

	TitleSceneController* const titleController{ cameraObject->AddComponent<TitleSceneController>() };
	titleController->SetCamera(camera);
}

void Scene_Title::OnUnload()
{
}
