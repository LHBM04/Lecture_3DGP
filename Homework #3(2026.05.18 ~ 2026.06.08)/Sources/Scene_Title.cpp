#include "Precompiled.h"
#include "Scene_Title.h"

#include "Animator.h"
#include "Camera.h"
#include "CubeCollider.h"
#include "GameObject.h"
#include "Light.h"
#include "Logger.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Quaternion.h"
#include "ResourceSystem.h"
#include "TitleController.h"
#include "Transform.h"
#include "Vector3D.h"
#include "Vector4D.h"

void Scene_Title::OnLoad()
{
	ResourceSystem& resourceSystem{ ResourceSystem::GetInstance() };

	GameObject* const cameraObject{ Instantiate(Vector3D(0.0f, 0.0f, -10.0f)) };
	cameraObject->SetName(L"Title Camera");

	Camera* const camera{ cameraObject->AddComponent<Camera>() };
	camera->SetViewport(Vector4D(0.0f, 0.0f, 1.0f, 1.0f));
	camera->SetFOV(60.0f);
	camera->SetNearClipPlane(0.1f);
	camera->SetFarClipPlane(1000.0f);

	Transform* const cameraTransform{ cameraObject->GetComponent<Transform>() };
	cameraTransform->SetWorldRotation(Quaternion::LookRotation(Vector3D::GetForward(), Vector3D::GetUp()));

	GameObject* const lightObject{ Instantiate() };
	lightObject->SetName(L"Title Light");
	lightObject->AddComponent<Light>();
	Transform* const lightTransform{ lightObject->GetComponent<Transform>() };
	lightTransform->SetLocalRotation(Quaternion::Euler(35.0f, 180.0f, 0.0f));

	GameObject* const controllerObject{ Instantiate() };
	controllerObject->SetName(L"Title Controller");
	TitleController* const controller{ controllerObject->AddComponent<TitleController>() };

	// 타이틀 로고
	{
		GameObject* const titleLogoObject{ Instantiate(Vector3D(0.0f, 0.0f, 10.0f)) };
		titleLogoObject->SetName(L"Title Logo");
		titleLogoObject->AddComponent<Animator>();

		MeshRenderer* const logoRenderer{ titleLogoObject->AddComponent<MeshRenderer>() };
		logoRenderer->SetMesh(resourceSystem.GetResource<Mesh>(L"Resources/Meshes/3D_GAME_PROGRAMMING_1.bin"));
		logoRenderer->SetMaterial(resourceSystem.GetResource<Material>(L"Resources/Materials/UI.bin"));

		CubeCollider* const logoCollider{ titleLogoObject->AddComponent<CubeCollider>() };
		logoCollider->SetCenter(Vector3D(-10.01f, -0.5f, 0.0f));
		logoCollider->SetSize(Vector3D(22.0f, 3.0f, 2.0f));
		titleLogoObject->AddComponent<Animator>();

		controller->SetTitleLogoObject(titleLogoObject);
	}

	// 튜토리얼
	{
		GameObject* const tutorialObject{ Instantiate(Vector3D(0.0f, 3.0f, 10.0f)) };
		tutorialObject->SetName(L"Tutorial");
		tutorialObject->AddComponent<Animator>();

		MeshRenderer* const tutorialRenderer{ tutorialObject->AddComponent<MeshRenderer>() };
		tutorialRenderer->SetMesh(resourceSystem.GetResource<Mesh>(L"Resources/Meshes/TUTORIAL.bin"));
		tutorialRenderer->SetMaterial(resourceSystem.GetResource<Material>(L"Resources/Materials/UI.bin"));

		CubeCollider* const tutorialCollider{ tutorialObject->AddComponent<CubeCollider>() };
		tutorialCollider->SetCenter(Vector3D(-10.01f, -0.5f, 0.0f));
		tutorialCollider->SetSize(Vector3D(22.0f, 3.0f, 2.0f));

		controller->SetTutorialButtonObject(tutorialObject);
	}

	// Level 1
	{
		GameObject* const level1Object{ Instantiate(Vector3D(-5.0f, 0.0f, 10.0f)) };
		level1Object->SetName(L"Level 1");
		level1Object->AddComponent<Animator>();

		MeshRenderer* const level1Renderer{ level1Object->AddComponent<MeshRenderer>() };
		level1Renderer->SetMesh(resourceSystem.GetResource<Mesh>(L"Resources/Meshes/LEVEL_1.bin"));
		level1Renderer->SetMaterial(resourceSystem.GetResource<Material>(L"Resources/Materials/UI.bin"));

		CubeCollider* const level1Collider{ level1Object->AddComponent<CubeCollider>() };
		level1Collider->SetCenter(Vector3D(-10.01f, -0.5f, 0.0f));
		level1Collider->SetSize(Vector3D(22.0f, 3.0f, 2.0f));

		controller->SetLevel1ButtonObject(level1Object);
	}

	// Level 2
	{
		GameObject* const level2Object{ Instantiate(Vector3D(0.0f, 0.0f, 10.0f)) };
		level2Object->SetName(L"Level 2");
		level2Object->AddComponent<Animator>();

		MeshRenderer* const level2Renderer{ level2Object->AddComponent<MeshRenderer>() };
		level2Renderer->SetMesh(resourceSystem.GetResource<Mesh>(L"Resources/Meshes/LEVEL_2.bin"));
		level2Renderer->SetMaterial(resourceSystem.GetResource<Material>(L"Resources/Materials/UI.bin"));

		CubeCollider* const level1Collider{ level2Object->AddComponent<CubeCollider>() };
		level1Collider->SetCenter(Vector3D(-10.01f, -0.5f, 0.0f));
		level1Collider->SetSize(Vector3D(22.0f, 3.0f, 2.0f));

		controller->SetLevel2ButtonObject(level2Object);
	}

	// Level 3
	{
		GameObject* const level3Object{ Instantiate(Vector3D(-5.0f, 3.0f, 10.0f)) };
		level3Object->SetName(L"Level 3");
		level3Object->AddComponent<Animator>();

		MeshRenderer* const level3Renderer{ level3Object->AddComponent<MeshRenderer>() };
		level3Renderer->SetMesh(resourceSystem.GetResource<Mesh>(L"Resources/Meshes/LEVEL_3.bin"));
		level3Renderer->SetMaterial(resourceSystem.GetResource<Material>(L"Resources/Materials/UI.bin"));

		CubeCollider* const level3Collider{ level3Object->AddComponent<CubeCollider>() };
		level3Collider->SetCenter(Vector3D(-10.01f, -0.5f, 0.0f));
		level3Collider->SetSize(Vector3D(22.0f, 3.0f, 2.0f));

		controller->SetLevel3ButtonObject(level3Object);
	}

	// Start
	{
		GameObject* const startObject{ Instantiate(Vector3D(-2.5f, -3.0f, 10.0f)) };
		startObject->SetName(L"Start");
		startObject->AddComponent<Animator>();

		MeshRenderer* const startRenderer{ startObject->AddComponent<MeshRenderer>() };
		startRenderer->SetMesh(resourceSystem.GetResource<Mesh>(L"Resources/Meshes/START.bin"));
		startRenderer->SetMaterial(resourceSystem.GetResource<Material>(L"Resources/Materials/UI.bin"));

		CubeCollider* const startCollider{ startObject->AddComponent<CubeCollider>() };
		startCollider->SetCenter(Vector3D(-10.01f, -0.5f, 0.0f));
		startCollider->SetSize(Vector3D(22.0f, 3.0f, 2.0f));

		controller->SetStartButtonObject(startObject);
	}

	// End
	{
		GameObject* const endObject{ Instantiate(Vector3D(2.5f, -3.0f, 10.0f)) };
		endObject->SetName(L"End");
		endObject->AddComponent<Animator>();

		MeshRenderer* const endRenderer{ endObject->AddComponent<MeshRenderer>() };
		endRenderer->SetMesh(resourceSystem.GetResource<Mesh>(L"Resources/Meshes/END.bin"));
		endRenderer->SetMaterial(resourceSystem.GetResource<Material>(L"Resources/Materials/UI.bin"));

		CubeCollider* const endCollider{ endObject->AddComponent<CubeCollider>() };
		endCollider->SetCenter(Vector3D(-10.01f, -0.5f, 0.0f));
		endCollider->SetSize(Vector3D(22.0f, 3.0f, 2.0f));

		controller->SetEndButtonObject(endObject);
	}
}

void Scene_Title::OnUnload()
{
}
