#include "Precompiled.h"
#include "Scene_Title.h"

#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshComponent.h"
#include "ResourceSystem.h"
#include "Mesh.h"
#include "Material.h"

void Scene_Title::OnLoad()
{
	GameObject* cameraObject{ CreateGameObject() };
	cameraObject->SetName(L"Main Camera");
	cameraObject->SetTag(L"MainCamera");

	Transform* cameraTransform{ cameraObject->GetComponent<Transform>() };
	cameraTransform->SetWorldPosition(Vector3D{ 0, 0, -10.0f });

	cameraObject->AddComponent<Camera>();

	// 테스트용 머터리얼과 메쉬 생성 및 컴포넌트 추가 예시
	auto* mesh = ResourceSystem::GetInstance().GetOrLoadResource<Mesh>(L"DefaultMesh");
	auto* mat = ResourceSystem::GetInstance().GetOrLoadResource<Material>(L"DefaultMat");
	
	GameObject* testObj = CreateGameObject();
	auto* meshComp = testObj->AddComponent<MeshComponent>();
	meshComp->SetMesh(mesh);
	meshComp->SetMaterial(mat);
}

void Scene_Title::OnUnload()
{

}
