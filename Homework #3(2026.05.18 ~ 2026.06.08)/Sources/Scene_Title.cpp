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

namespace
{
	void ConfigureUICollider(CubeCollider* collider_, const Mesh* mesh_)
	{
		if (collider_ == nullptr || mesh_ == nullptr)
		{
			return;
		}

		const std::vector<Vertex>& vertices{ mesh_->GetVertices() };
		if (vertices.empty())
		{
			return;
		}

		Vector3D boundsMin{ vertices.front().position };
		Vector3D boundsMax{ vertices.front().position };
		for (const Vertex& vertex : vertices)
		{
			boundsMin = Vector3D::Min(boundsMin, vertex.position);
			boundsMax = Vector3D::Max(boundsMax, vertex.position);
		}

		const Vector3D boundsCenter{ (boundsMin + boundsMax) * 0.5f };
		const Vector3D boundsExtents{ (boundsMax - boundsMin) * 0.5f };
		const float colliderDepth{ boundsExtents.z > 1.0f ? boundsExtents.z * 2.0f : 2.0f };
		collider_->SetCenter(boundsCenter);
		collider_->SetSize(Vector3D(boundsExtents.x * 2.0f, boundsExtents.y * 2.0f, colliderDepth));
		collider_->UpdateVolume();
	}
}

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

		Mesh* const logoMesh{ resourceSystem.GetResource<Mesh>(L"Resources/Meshes/3D_GAME_PROGRAMMING_1.bin") };
		MeshRenderer* const logoRenderer{ titleLogoObject->AddComponent<MeshRenderer>() };
		logoRenderer->SetMesh(logoMesh);
		logoRenderer->SetMaterial(resourceSystem.GetResource<Material>(L"Resources/Materials/UI.bin"));

		CubeCollider* const logoCollider{ titleLogoObject->AddComponent<CubeCollider>() };
		ConfigureUICollider(logoCollider, logoMesh);

		controller->SetTitleLogoObject(titleLogoObject);
	}

	// 튜토리얼
	{
		GameObject* const tutorialObject{ Instantiate(Vector3D(-5.0f, 2.5f, 10.0f)) };
		tutorialObject->SetName(L"Tutorial");
		tutorialObject->AddComponent<Animator>();

		Mesh* const tutorialMesh{ resourceSystem.GetResource<Mesh>(L"Resources/Meshes/TUTORIAL.bin") };
		MeshRenderer* const tutorialRenderer{ tutorialObject->AddComponent<MeshRenderer>() };
		tutorialRenderer->SetMesh(tutorialMesh);
		tutorialRenderer->SetMaterial(resourceSystem.GetResource<Material>(L"Resources/Materials/UI.bin"));

		CubeCollider* const tutorialCollider{ tutorialObject->AddComponent<CubeCollider>() };
		ConfigureUICollider(tutorialCollider, tutorialMesh);

		controller->SetTutorialButtonObject(tutorialObject);
	}

	// Level 1
	{
		GameObject* const level1Object{ Instantiate(Vector3D(5.0f, 2.5f, 10.0f)) };
		level1Object->SetName(L"Level 1");
		level1Object->AddComponent<Animator>();

		Mesh* const level1Mesh{ resourceSystem.GetResource<Mesh>(L"Resources/Meshes/LEVEL_1.bin") };
		MeshRenderer* const level1Renderer{ level1Object->AddComponent<MeshRenderer>() };
		level1Renderer->SetMesh(level1Mesh);
		level1Renderer->SetMaterial(resourceSystem.GetResource<Material>(L"Resources/Materials/UI.bin"));

		CubeCollider* const level1Collider{ level1Object->AddComponent<CubeCollider>() };
		ConfigureUICollider(level1Collider, level1Mesh);

		controller->SetLevel1ButtonObject(level1Object);
	}

	// Level 2
	{
		GameObject* const level2Object{ Instantiate(Vector3D(-5.0f, -2.5f, 10.0f)) };
		level2Object->SetName(L"Level 2");
		level2Object->AddComponent<Animator>();

		Mesh* const level2Mesh{ resourceSystem.GetResource<Mesh>(L"Resources/Meshes/LEVEL_2.bin") };
		MeshRenderer* const level2Renderer{ level2Object->AddComponent<MeshRenderer>() };
		level2Renderer->SetMesh(level2Mesh);
		level2Renderer->SetMaterial(resourceSystem.GetResource<Material>(L"Resources/Materials/UI.bin"));

		CubeCollider* const level1Collider{ level2Object->AddComponent<CubeCollider>() };
		ConfigureUICollider(level1Collider, level2Mesh);

		controller->SetLevel2ButtonObject(level2Object);
	}

	// Level 3
	{
		GameObject* const level3Object{ Instantiate(Vector3D(5.0f, -2.5f, 10.0f)) };
		level3Object->SetName(L"Level 3");
		level3Object->AddComponent<Animator>();

		Mesh* const level3Mesh{ resourceSystem.GetResource<Mesh>(L"Resources/Meshes/LEVEL_3.bin") };
		MeshRenderer* const level3Renderer{ level3Object->AddComponent<MeshRenderer>() };
		level3Renderer->SetMesh(level3Mesh);
		level3Renderer->SetMaterial(resourceSystem.GetResource<Material>(L"Resources/Materials/UI.bin"));

		CubeCollider* const level3Collider{ level3Object->AddComponent<CubeCollider>() };
		ConfigureUICollider(level3Collider, level3Mesh);

		controller->SetLevel3ButtonObject(level3Object);
	}

	// Start
	{
		GameObject* const startObject{ Instantiate(Vector3D(-3.0f, -7.5f, 10.0f)) };
		startObject->SetName(L"Start");
		startObject->AddComponent<Animator>();

		Mesh* const startMesh{ resourceSystem.GetResource<Mesh>(L"Resources/Meshes/START.bin") };
		MeshRenderer* const startRenderer{ startObject->AddComponent<MeshRenderer>() };
		startRenderer->SetMesh(startMesh);
		startRenderer->SetMaterial(resourceSystem.GetResource<Material>(L"Resources/Materials/UI.bin"));

		CubeCollider* const startCollider{ startObject->AddComponent<CubeCollider>() };
		ConfigureUICollider(startCollider, startMesh);

		controller->SetStartButtonObject(startObject);
	}

	// End
	{
		GameObject* const endObject{ Instantiate(Vector3D(3.0f, -7.5f, 10.0f)) };
		endObject->SetName(L"End");
		endObject->AddComponent<Animator>();

		Mesh* const endMesh{ resourceSystem.GetResource<Mesh>(L"Resources/Meshes/END.bin") };
		MeshRenderer* const endRenderer{ endObject->AddComponent<MeshRenderer>() };
		endRenderer->SetMesh(endMesh);
		endRenderer->SetMaterial(resourceSystem.GetResource<Material>(L"Resources/Materials/UI.bin"));

		CubeCollider* const endCollider{ endObject->AddComponent<CubeCollider>() };
		ConfigureUICollider(endCollider, endMesh);

		controller->SetEndButtonObject(endObject);
	}
}

void Scene_Title::OnUnload()
{
}
