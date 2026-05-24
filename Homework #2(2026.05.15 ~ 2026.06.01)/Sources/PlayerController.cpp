#include "Precompiled.h"
#include "PlayerController.h"

#include "GameObject.h"
#include "InputManager.h"
#include "Quaternion.h"
#include "Transform.h"
#include "Vector3D.h"

void PlayerController::OnUpdate()
{
	GameObject* owner{ GetOwner() };
	if (nullptr == owner)
	{
		return;
	}

	Transform* transform{ owner->GetComponent<Transform>() };
	if (nullptr == transform)
	{
		return;
	}

	float yawDelta{ 0.0f };
	if (InputManager::IsKeyDown(KeyCode::A) || InputManager::IsKeyDown(KeyCode::Left))
	{
		yawDelta -= rotationSpeed;
	}

	if (InputManager::IsKeyDown(KeyCode::D) || InputManager::IsKeyDown(KeyCode::Right))
	{
		yawDelta += rotationSpeed;
	}

	if (0.0f == yawDelta)
	{
		return;
	}

	const Quaternion rotationDelta{ Quaternion::Euler(0.0f, yawDelta, 0.0f) };
	transform->SetLocalRotation(transform->GetLocalRotation() * rotationDelta);
}
