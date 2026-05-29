#include "Precompiled.h"
#include "Scene_Title.h"

#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"

void Scene_Title::OnLoad()
{
	GameObject* cameraObject{ CreateGameObject() };
	cameraObject->SetName(L"Main Camera");
	cameraObject->SetTag(L"MainCamera");

	Transform* cameraTransform{ cameraObject->GetComponent<Transform>() };
	cameraTransform->SetWorldPosition(Vector3D{0, 0, -10.0f});

	cameraObject->AddComponent<Camera>();
}

void Scene_Title::OnUnload()
{

}
