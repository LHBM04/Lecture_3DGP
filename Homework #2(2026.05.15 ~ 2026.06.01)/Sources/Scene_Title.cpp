#include "Precompiled.h"

#include "Scene_Title.h"

#include "Camera.h"
#include "GameObject.h"
#include "Light.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "ResourceSystem.h"
#include "TitleSceneController.h"
#include "CubeCollider.h"
#include "Transform.h"

void Scene_Title::OnLoad()
{
	GameObject* cameraObject{ Instantiate() };
	cameraObject->SetName(L"Main Camera");
	cameraObject->SetTag(L"MainCamera");

	Transform* cameraTransform{ cameraObject->GetComponent<Transform>() };
	cameraTransform->SetWorldPosition(Vector3D{ 0.0f, 0.0f, -20.0f });
	cameraTransform->SetLocalRotation(Quaternion::Euler(0.0f, 0.0f, 0.0f));

	Camera* camera{ cameraObject->AddComponent<Camera>() };

	GameObject* lightObject{ Instantiate() };
	lightObject->SetName(L"Main Light");

	Transform* lightTransform{ lightObject->GetComponent<Transform>() };
	lightTransform->SetLocalRotation(Quaternion::Euler(45.0f, -45.0f, 0.0f));

	Light* light{ lightObject->AddComponent<Light>() };
	light->SetIntensity(1.0f);
	light->SetColor(ColorRGBA{ 1.0f, 1.0f, 1.0f, 1.0f });

	Material* mat{ ResourceSystem::GetInstance().GetResource<Material>(L"DefaultMaterial") };
	if (mat != nullptr)
	{
		mat->SetBaseColor(Vector4D{ 1.0f, 1.0f, 1.0f, 1.0f });
	}

	Mesh* titleMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Title.bin") };
	Mesh* buttonMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Button.bin") };

	GameObject* titleObject{ Instantiate() };
	titleObject->SetName(L"TitleLogo");

	Transform* titleTransform{ titleObject->GetComponent<Transform>() };
	titleTransform->SetLocalPosition(Vector3D{ 0.0f, 4.0f, 75.0f });
	titleTransform->SetLocalScale(Vector3D{ 10.0f, 10.0f, 10.0f });
	
	MeshRenderer* titleRenderer{ titleObject->AddComponent<MeshRenderer>() };
	titleRenderer->SetMesh(titleMesh);
	titleRenderer->SetMaterial(mat);

	GameObject* buttonObject{ Instantiate() };
	buttonObject->SetName(L"StartButton");
	buttonObject->SetTag(L"StartButton");
	
	Transform* buttonTransform{ buttonObject->GetComponent<Transform>() };
	buttonTransform->SetLocalPosition(Vector3D{ 0.0f, -4.0f, 75.0f });
	buttonTransform->SetLocalScale(Vector3D{ 10.0f, 10.0f, 10.0f });

	MeshRenderer* buttonRenderer{ buttonObject->AddComponent<MeshRenderer>() };
	buttonRenderer->SetMesh(buttonMesh);
	buttonRenderer->SetMaterial(mat);

	if (buttonMesh != nullptr)
	{
		const Vector3D boundsMin{ buttonMesh->GetBoundsMin() };
		const Vector3D boundsMax{ buttonMesh->GetBoundsMax() };
		CubeCollider* buttonCollider{ buttonObject->AddComponent<CubeCollider>() };
		buttonCollider->SetCenter((boundsMin + boundsMax) * 0.5f);
		buttonCollider->SetSize(boundsMax - boundsMin);
		buttonCollider->SetStatic(true);
		buttonCollider->UpdateVolume();
	}

	TitleSceneController* titleController{ cameraObject->AddComponent<TitleSceneController>() };
	titleController->SetCamera(camera);
}

void Scene_Title::OnUnload()
{
}
