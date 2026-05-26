#include "Precompiled.h"
#include "PlayerController.h"

#include "GameObject.h"
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

	if (std::abs(currentInput.yaw) > Mathf::EPSILON)
	{
		const float yawDelta{ currentInput.yaw * rotationSpeed * deltaTime };
		const Quaternion rotationDelta{ Quaternion::Euler(0.0f, yawDelta, 0.0f) };
		transform->SetLocalRotation(transform->GetLocalRotation() * rotationDelta);
	}

	const Quaternion currentRotation{ Quaternion::Normalize(transform->GetWorldRotation()) };
	const Vector3D forward{ currentRotation * Vector3D::GetForward() };

	if (std::abs(currentInput.move) > Mathf::EPSILON)
	{
		const Vector3D currentPosition{ transform->GetWorldPosition() };
		const Vector3D nextPosition{ currentPosition + forward * (currentInput.move * moveSpeed * deltaTime) };
		transform->SetWorldPosition(nextPosition);
	}
}

void PlayerController::OnPlayerInput(const PlayerInput& input_)
{
	currentInput = input_;
}
