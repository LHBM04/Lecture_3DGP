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

	Material* const uiMaterial{ resourceSystem.GetResource<Material>(L"Resources/Materials/UI.bin") };
	
	constexpr float uiZ{ 10.0f };
	constexpr float logoScale{ 0.6f };
	constexpr float messageScale{ 0.4f };
	constexpr float tutorialScale{ 0.48f };
	constexpr float levelScale{ 0.44f };
	constexpr float actionScale{ 0.5f };

	GameObject* const titleLogoObject{ Instantiate(Vector3D(10.01f * logoScale, 4.8f, uiZ)) };
	titleLogoObject->SetName(L"Title Logo");
	MeshRenderer* const logoRenderer{ titleLogoObject->AddComponent<MeshRenderer>() };
	logoRenderer->SetMesh(resourceSystem.GetResource<Mesh>(L"Resources/Meshes/3D_GAME_PROGRAMMING_1.bin"));
	logoRenderer->SetMaterial(uiMaterial);
	CubeCollider* const logoCollider{ titleLogoObject->AddComponent<CubeCollider>() };
	logoCollider->SetCenter(Vector3D(-10.01f, -0.5f, 0.0f));
	logoCollider->SetSize(Vector3D(22.0f, 3.0f, 2.0f));
	titleLogoObject->AddComponent<Animator>();
	Transform* const logoTransform{ titleLogoObject->GetComponent<Transform>() };
	logoTransform->SetLocalRotation(Quaternion::GetIdentity());
	logoTransform->SetLocalScale(Vector3D(logoScale, logoScale, logoScale));

	GameObject* const messageObject{ Instantiate(Vector3D(2.42f * messageScale, 3.35f, uiZ)) };
	messageObject->SetName(L"Title Message");
	MeshRenderer* const messageRenderer{ messageObject->AddComponent<MeshRenderer>() };
	messageRenderer->SetMesh(resourceSystem.GetResource<Mesh>(L"Resources/Meshes/Title_Message.bin"));
	messageRenderer->SetMaterial(uiMaterial);
	messageObject->AddComponent<Animator>();
	Transform* const messageTransform{ messageObject->GetComponent<Transform>() };
	messageTransform->SetLocalRotation(Quaternion::GetIdentity());
	messageTransform->SetLocalScale(Vector3D(messageScale, messageScale, messageScale));

	constexpr float tutorialY{ 1.1f };
	constexpr float levelY{ -1.25f };
	constexpr float actionY{ -4.2f };
	constexpr float levelSpacing{ 4.7f };
	constexpr float actionSpacing{ 3.0f };

	GameObject* const tutorialButtonObject{ Instantiate(Vector3D(3.67f * tutorialScale, tutorialY, uiZ)) };
	tutorialButtonObject->SetName(L"Tutorial Button");
	MeshRenderer* const tutorialRenderer{ tutorialButtonObject->AddComponent<MeshRenderer>() };
	tutorialRenderer->SetMesh(resourceSystem.GetResource<Mesh>(L"Resources/Meshes/TUTORIAL.bin"));
	tutorialRenderer->SetMaterial(uiMaterial);
	CubeCollider* const tutorialCollider{ tutorialButtonObject->AddComponent<CubeCollider>() };
	tutorialCollider->SetCenter(Vector3D(-3.67f, -0.5f, 0.0f));
	tutorialCollider->SetSize(Vector3D(8.6f, 2.0f, 2.0f));
	tutorialButtonObject->AddComponent<Animator>();
	Transform* const tutorialTransform{ tutorialButtonObject->GetComponent<Transform>() };
	tutorialTransform->SetLocalRotation(Quaternion::GetIdentity());
	tutorialTransform->SetLocalScale(Vector3D(tutorialScale, tutorialScale, tutorialScale));

	GameObject* const level1ButtonObject{ Instantiate(Vector3D(-levelSpacing + 2.84f * levelScale, levelY, uiZ)) };
	level1ButtonObject->SetName(L"Level1 Button");
	MeshRenderer* const level1Renderer{ level1ButtonObject->AddComponent<MeshRenderer>() };
	level1Renderer->SetMesh(resourceSystem.GetResource<Mesh>(L"Resources/Meshes/LEVEL_1.bin"));
	level1Renderer->SetMaterial(uiMaterial);
	CubeCollider* const level1Collider{ level1ButtonObject->AddComponent<CubeCollider>() };
	level1Collider->SetCenter(Vector3D(-2.84f, -0.5f, 0.0f));
	level1Collider->SetSize(Vector3D(6.8f, 2.0f, 2.0f));
	level1ButtonObject->AddComponent<Animator>();
	Transform* const level1Transform{ level1ButtonObject->GetComponent<Transform>() };
	level1Transform->SetLocalRotation(Quaternion::GetIdentity());
	level1Transform->SetLocalScale(Vector3D(levelScale, levelScale, levelScale));

	GameObject* const level2ButtonObject{ Instantiate(Vector3D(2.83f * levelScale, levelY, uiZ)) };
	level2ButtonObject->SetName(L"Level2 Button");
	MeshRenderer* const level2Renderer{ level2ButtonObject->AddComponent<MeshRenderer>() };
	level2Renderer->SetMesh(resourceSystem.GetResource<Mesh>(L"Resources/Meshes/LEVEL_2.bin"));
	level2Renderer->SetMaterial(uiMaterial);
	CubeCollider* const level2Collider{ level2ButtonObject->AddComponent<CubeCollider>() };
	level2Collider->SetCenter(Vector3D(-2.83f, -0.5f, 0.0f));
	level2Collider->SetSize(Vector3D(6.8f, 2.0f, 2.0f));
	level2ButtonObject->AddComponent<Animator>();
	Transform* const level2Transform{ level2ButtonObject->GetComponent<Transform>() };
	level2Transform->SetLocalRotation(Quaternion::GetIdentity());
	level2Transform->SetLocalScale(Vector3D(levelScale, levelScale, levelScale));

	GameObject* const level3ButtonObject{ Instantiate(Vector3D(levelSpacing + 2.83f * levelScale, levelY, uiZ)) };
	level3ButtonObject->SetName(L"Level3 Button");
	MeshRenderer* const level3Renderer{ level3ButtonObject->AddComponent<MeshRenderer>() };
	level3Renderer->SetMesh(resourceSystem.GetResource<Mesh>(L"Resources/Meshes/LEVEL_3.bin"));
	level3Renderer->SetMaterial(uiMaterial);
	CubeCollider* const level3Collider{ level3ButtonObject->AddComponent<CubeCollider>() };
	level3Collider->SetCenter(Vector3D(-2.83f, -0.5f, 0.0f));
	level3Collider->SetSize(Vector3D(6.8f, 2.0f, 2.0f));
	level3ButtonObject->AddComponent<Animator>();
	Transform* const level3Transform{ level3ButtonObject->GetComponent<Transform>() };
	level3Transform->SetLocalRotation(Quaternion::GetIdentity());
	level3Transform->SetLocalScale(Vector3D(levelScale, levelScale, levelScale));

	GameObject* const startButtonObject{ Instantiate(Vector3D(-actionSpacing + 2.27f * actionScale, actionY, uiZ)) };
	startButtonObject->SetName(L"Start Button");
	MeshRenderer* const startRenderer{ startButtonObject->AddComponent<MeshRenderer>() };
	startRenderer->SetMesh(resourceSystem.GetResource<Mesh>(L"Resources/Meshes/START.bin"));
	startRenderer->SetMaterial(uiMaterial);
	CubeCollider* const startCollider{ startButtonObject->AddComponent<CubeCollider>() };
	startCollider->SetCenter(Vector3D(-2.27f, -0.5f, 0.0f));
	startCollider->SetSize(Vector3D(5.8f, 2.0f, 2.0f));
	startButtonObject->AddComponent<Animator>();
	Transform* const startTransform{ startButtonObject->GetComponent<Transform>() };
	startTransform->SetLocalRotation(Quaternion::GetIdentity());
	startTransform->SetLocalScale(Vector3D(actionScale, actionScale, actionScale));

	GameObject* const endButtonObject{ Instantiate(Vector3D(actionSpacing + 1.46f * actionScale, actionY, uiZ)) };
	endButtonObject->SetName(L"End Button");
	MeshRenderer* const endRenderer{ endButtonObject->AddComponent<MeshRenderer>() };
	endRenderer->SetMesh(resourceSystem.GetResource<Mesh>(L"Resources/Meshes/END.bin"));
	endRenderer->SetMaterial(uiMaterial);
	CubeCollider* const endCollider{ endButtonObject->AddComponent<CubeCollider>() };
	endCollider->SetCenter(Vector3D(-1.46f, -0.5f, 0.0f));
	endCollider->SetSize(Vector3D(4.2f, 2.0f, 2.0f));
	endButtonObject->AddComponent<Animator>();
	Transform* const endTransform{ endButtonObject->GetComponent<Transform>() };
	endTransform->SetLocalRotation(Quaternion::GetIdentity());
	endTransform->SetLocalScale(Vector3D(actionScale, actionScale, actionScale));

	controller->SetTitleLogoObject(titleLogoObject);
	controller->SetTutorialButtonObject(tutorialButtonObject);
	controller->SetLevel1ButtonObject(level1ButtonObject);
	controller->SetLevel2ButtonObject(level2ButtonObject);
	controller->SetLevel3ButtonObject(level3ButtonObject);
	controller->SetStartButtonObject(startButtonObject);
	controller->SetEndButtonObject(endButtonObject);
}

void Scene_Title::OnUnload()
{
}
