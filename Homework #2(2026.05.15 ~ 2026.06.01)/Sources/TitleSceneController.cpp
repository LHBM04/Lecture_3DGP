#include "Precompiled.h"
#include "TitleSceneController.h"

#include "Camera.h"
#include "GameObject.h"
#include "InputSystem.h"
#include "PhysicsSystem.h"
#include "SceneSystem.h"
#include "Vector2D.h"
#include "Vector3D.h"

void TitleSceneController::OnUpdate(float)
{
	if (camera == nullptr)
	{
		return;
	}

	if (!InputSystem::GetInstance().IsButtonPressed(ButtonCode::Left))
	{
		return;
	}

	Vector3D rayOrigin;
	Vector3D rayDir;
	camera->ScreenPointToRay(InputSystem::GetInstance().GetMousePosition(), rayOrigin, rayDir);

	GameObject* hitObject{ PhysicsSystem::GetInstance().Raycast(rayOrigin, rayDir) };
	if (hitObject != nullptr && hitObject->GetTag() == L"StartButton")
	{
		SceneSystem::GetInstance().LoadScene(L"Stage 1");
	}
}

void TitleSceneController::SetCamera(Camera* camera_) noexcept
{
	camera = camera_;
}

