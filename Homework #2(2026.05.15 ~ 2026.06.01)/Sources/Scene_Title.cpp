#include "Precompiled.h"
#include "Scene_Title.h"

#include "GameObject.h"
#include "Transform.h"

void Scene_Title::OnLoad()
{
	GameObject* cameraObject{ CreateGameObject() };
	cameraObject->SetName(L"Main Camera");
	cameraObject->SetTag(L"MainCamera");

	Transform* cameraTransform{ cameraObject->AddComponent<Transform>() };
	cameraTransform->SetWorldPosition(Vector3D{0, 0, -10.0f});
}

void Scene_Title::OnUnload()
{

}
