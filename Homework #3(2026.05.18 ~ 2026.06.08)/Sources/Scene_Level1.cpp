#include "Precompiled.h"
#include "Scene_Level1.h"

#include "Animator.h"
#include "AnimationClip.h"
#include "Camera.h"
#include "GameObject.h"
#include "Logger.h"
#include "Model.h"
#include "Quaternion.h"
#include "ResourceSystem.h"
#include "Transform.h"
#include "Vector3D.h"
#include "Vector4D.h"

void Scene_Level1::OnLoad()
{
	ResourceSystem& resourceSystem{ ResourceSystem::GetInstance() };

	GameObject* const cameraObject{ Instantiate(Vector3D(0.0f, 0.0f, -10.0f)) };
	cameraObject->SetName(L"Main Camera");

	Camera* const camera{ cameraObject->AddComponent<Camera>() };
	camera->SetViewport(Vector4D(0.0f, 0.0f, 1.0f, 1.0f));
	camera->SetFOV(60.0f);
	camera->SetNearClipPlane(0.1f);
	camera->SetFarClipPlane(1000.0f);

	Model* const model{ resourceSystem.GetResource<Model>(L"Resources/Models/Apache.bin") };
	if (model == nullptr)
	{
		Logger::Critical(L"Model not found: Resources/Models/Apache.bin");
		return;
	}

	GameObject* const modelObject{ Instantiate(model) };
	
	Transform* const modelTransform{ modelObject->GetComponent<Transform>() };
	//modelTransform->SetWorldScale(Vector3D(0.01f, 0.01f, 0.01f));
	modelTransform->SetWorldPosition(Vector3D(0.0f, 0.0f, 50.0f));

	AnimationClip* const idleAnim{ resourceSystem.GetResource<AnimationClip>(L"Resources/Animations/Idle.bin") };
	if (idleAnim != nullptr)
	{
		Animator* const animator{ modelObject->AddComponent<Animator>() };
		animator->Play(idleAnim, true);
	}
	else
	{
		Logger::Warning(L"Animation not found: Resources/Animations/Idle.bin");
	}
}

void Scene_Level1::OnUnload()
{
}
