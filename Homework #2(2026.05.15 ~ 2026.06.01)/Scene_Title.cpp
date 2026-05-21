#include "Precompiled.h"
#include "Scene_Title.h"

#include "AssetManager.h"
#include "Camera.h"
#include "Font.h"
#include "GameObject.h"
#include "ImageView.h"
#include "Input.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "RectTransform.h"
#include "TextView.h"
#include "Timer.h"
#include "Transform.h"

GameObject* cubeObject{ nullptr };
GameObject* cameraObject{ nullptr };

void Scene_Title::OnLoad()
{
	cube = AssetManager::Load<Mesh>(L"Assets/Cube.obj");
	font = AssetManager::Load<Font>(L"Assets/Builtin.font");
	if (nullptr != cube)
	{
		cubeObject = AddGameObject();
		cubeObject->SetName("Cube");

		MeshRenderer* const meshRenderer = cubeObject->AddComponent<MeshRenderer>();
		meshRenderer->SetMesh(cube);
	}

	cameraObject = AddGameObject();
	cameraObject->SetName("Main Camera");
	Camera* const camera = cameraObject->AddComponent<Camera>();
	SetMainCamera(camera);

	Transform* const cameraTransform = cameraObject->GetTransform();
	const Vector3D cameraPosition{ 0.0f, 0.0f, -10.0f };
	cameraTransform->SetWorldPosition(cameraPosition);

	GameObject* const testPanel = AddUIGameObject();
	testPanel->SetName("Test UI Panel");

	RectTransform* const rectTransform = testPanel->GetComponent<RectTransform>();
	rectTransform->SetAnchoredPosition(Vector2D(0.0f, 0.0f));
	rectTransform->SetSizeDelta(Vector2D(240.0f, 120.0f));

	ImageView* const imageView = testPanel->AddComponent<ImageView>();
	imageView->SetColor(ColorRGBA(0.95f, 0.35f, 0.15f, 0.85f));

	GameObject* const testText = AddUIGameObject();
	testText->SetName("Test UI Text");

	RectTransform* const textRect = testText->GetComponent<RectTransform>();
	textRect->SetAnchoredPosition(Vector2D(-95.0f, 8.0f));
	textRect->SetSizeDelta(Vector2D(200.0f, 50.0f));
	textRect->SetPivot(Vector2D(0.0f, 0.5f));

	TextView* const textView = testText->AddComponent<TextView>();
	textView->SetText("HELLO UI");
	textView->SetFont(font);
	textView->SetPixelSize(4.0f);
	textView->SetColor(ColorRGBA::GetWhite());
}

void Scene_Title::OnUpdate()
{
	auto transform = cubeObject->GetTransform();
	if (nullptr == transform)
	{
		return;
	}

	// const float rotationSpeed = 90.0f; // degrees per second
	// transform->SetLocalRotation(Quaternion::AngleAxis(rotationSpeed * Timer::GetTotalTime(), Vector3D::GetUp()));
	// 
	// //transform->SetWorldPosition(transform->GetWorldPosition() + Vector3D::GetUp() * 3.0f * Timer::GetDeltaTime());
	// 
	if (Input::IsKeyHeld('W'))
	{
		transform->SetWorldPosition(transform->GetWorldPosition() + Vector3D::GetForward() * 3.0f * Timer::GetDeltaTime());
	}
	if (Input::IsKeyHeld('S'))
	{
		transform->SetWorldPosition(transform->GetWorldPosition() + Vector3D::GetBack() * 3.0f * Timer::GetDeltaTime());
	}
	if (Input::IsKeyHeld('A'))
	{
		transform->SetWorldPosition(transform->GetWorldPosition() + Vector3D::GetLeft() * 3.0f * Timer::GetDeltaTime());
	}
	if (Input::IsKeyHeld('D'))
	{
		transform->SetWorldPosition(transform->GetWorldPosition() + Vector3D::GetRight() * 3.0f * Timer::GetDeltaTime());
	}

	// if (nullptr != cameraObject)
	// {
	// 	Transform* const cameraTransform = cameraObject->GetTransform();
	// 	cameraTransform->SetWorldRotation(Quaternion::LookRotation(Vector3D::Normalize(transform->GetWorldPosition() - cameraTransform->GetWorldPosition()), Vector3D::GetUp()));
	// }
}

void Scene_Title::OnUnload()
{
	AssetManager::Unload(cube);
	cube = nullptr;
	AssetManager::Unload(font);
	font = nullptr;
}
