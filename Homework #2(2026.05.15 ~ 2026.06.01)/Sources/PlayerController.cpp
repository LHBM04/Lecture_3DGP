#include "Precompiled.h"
#include "PlayerController.h"

#include "GameObject.h"
#include "InputManager.h"
#include "Quaternion.h"
#include "Timer.h"
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

	const float deltaTime{ Timer::GetDeltaTime() };

	float yawInput{ 0.0f };
	if (InputManager::IsKeyDown(KeyCode::A) || InputManager::IsKeyDown(KeyCode::Left))
	{
		yawInput -= 1.0f;
	}

	if (InputManager::IsKeyDown(KeyCode::D) || InputManager::IsKeyDown(KeyCode::Right))
	{
		yawInput += 1.0f;
	}

	if (std::abs(yawInput) > Mathf::EPSILON)
	{
		const float yawDelta{ yawInput * rotationSpeed * deltaTime };
		const Quaternion rotationDelta{ Quaternion::Euler(0.0f, yawDelta, 0.0f) };
		transform->SetLocalRotation(transform->GetLocalRotation() * rotationDelta);
	}

	const Quaternion currentRotation{ Quaternion::Normalize(transform->GetWorldRotation()) };
	const Vector3D forward{ currentRotation * Vector3D::GetForward() };

	float moveInput{ 0.0f };
	if (InputManager::IsKeyDown(KeyCode::W) || InputManager::IsKeyDown(KeyCode::Up))
	{
		moveInput += 1.0f;
	}
	if (InputManager::IsKeyDown(KeyCode::S) || InputManager::IsKeyDown(KeyCode::Down))
	{
		moveInput -= 1.0f;
	}

	if (std::abs(moveInput) > Mathf::EPSILON)
	{
		const Vector3D currentPosition{ transform->GetWorldPosition() };
		const Vector3D nextPosition{ currentPosition + forward * (moveInput * moveSpeed * deltaTime) };
		transform->SetWorldPosition(nextPosition);
	}
}
