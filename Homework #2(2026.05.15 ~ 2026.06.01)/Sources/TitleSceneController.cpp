#include "Precompiled.h"

#include "TitleSceneController.h"

#include "Camera.h"
#include "GameObject.h"
#include "InputSystem.h"
#include "PhysicsSystem.h"
#include "Scene.h"
#include "SceneSystem.h"
#include "Vector2D.h"
#include "Vector3D.h"

void TitleSceneController::OnUpdate(float deltaTime_)
{
	if (InputSystem::GetInstance().IsButtonPressed(ButtonCode::Left))
	{
		const Vector2D mousePos{ InputSystem::GetInstance().GetMousePosition() };
		Vector3D rayOrigin{ Vector3D::GetZero() };
		Vector3D rayDir{ Vector3D::GetForward() };

		if (camera != nullptr)
		{
			camera->ScreenPointToRay(mousePos, rayOrigin, rayDir);
		}

		float distance{ 0.0f };
		GameObject* const hitObject{ PhysicsSystem::GetInstance().Raycast(rayOrigin, rayDir, &distance) };

		if (hitObject != nullptr && hitObject->GetName() == L"Start Button")
		{
			SceneSystem::GetInstance().LoadScene(L"Menu Scene");
		}
	}
}

void TitleSceneController::SetCamera(Camera* camera_) noexcept
{
	camera = camera_;
}
