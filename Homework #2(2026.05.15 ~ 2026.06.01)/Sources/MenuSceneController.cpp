#include "Precompiled.h"
#include "MenuSceneController.h"

#include "Camera.h"
#include "GameObject.h"
#include "InputSystem.h"
#include "PhysicsSystem.h"
#include "Scene.h"
#include "SceneSystem.h"
#include "Vector2D.h"
#include "Vector3D.h"

void MenuSceneController::OnUpdate(float deltaTime_)
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

		if (hitObject != nullptr)
		{
			// Stage Day
			if (hitObject->GetName() == L"Day")
			{
				SceneSystem::GetInstance().LoadScene(L"Stage 1");
			}
			// Stage Night
			else if (hitObject->GetName() == L"Night")
			{
				SceneSystem::GetInstance().LoadScene(L"Stage 2");
			}
		}
	}
}

void MenuSceneController::SetCamera(Camera* camera_) noexcept
{
	camera = camera_;
}
