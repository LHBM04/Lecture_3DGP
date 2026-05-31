#include "Precompiled.h"

#include "MenuSceneController.h"

#include "Camera.h"
#include "GameObject.h"
#include "InputSystem.h"
#include "PhysicsSystem.h"
#include "SceneSystem.h"

void MenuSceneController::OnUpdate(float)
{
	if (camera == nullptr)
	{
		return;
	}

	if (!InputSystem::GetInstance().IsButtonPressed(ButtonCode::Left))
	{
		return;
	}

	Vector3D rayOrigin{};
	Vector3D rayDir{};
	camera->ScreenPointToRay(InputSystem::GetInstance().GetMousePosition(), rayOrigin, rayDir);

	GameObject* hitObject{ PhysicsSystem::GetInstance().Raycast(rayOrigin, rayDir) };
	if (hitObject == nullptr)
	{
		return;
	}

	const std::wstring_view name{ hitObject->GetName() };
	if (name == L"Day")
	{
		SceneSystem::GetInstance().LoadScene(L"Stage 1");
	}
	else if (name == L"Night")
	{
		SceneSystem::GetInstance().LoadScene(L"Stage 2");
	}
}

void MenuSceneController::SetCamera(Camera* camera_) noexcept
{
	camera = camera_;
}
