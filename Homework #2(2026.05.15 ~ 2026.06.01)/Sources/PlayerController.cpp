#include "Precompiled.h"
#include "PlayerController.h"

#include "GameObject.h"
#include "Quaternion.h"
#include "TimeContext.h"
#include "Transform.h"
#include "Vector3D.h"

void PlayerController::OnUpdate(const TimeContext& context_)
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

	const float deltaTime{ context_.deltaTime };

	if (std::abs(yawInput) > Mathf::EPSILON)
	{
		const float yawDelta{ yawInput * rotationSpeed * deltaTime };
		const Quaternion rotationDelta{ Quaternion::Euler(0.0f, yawDelta, 0.0f) };
		transform->SetLocalRotation(transform->GetLocalRotation() * rotationDelta);
	}

	const Quaternion currentRotation{ Quaternion::Normalize(transform->GetWorldRotation()) };
	const Vector3D forward{ currentRotation * Vector3D::GetForward() };

	if (std::abs(moveInput) > Mathf::EPSILON)
	{
		const Vector3D currentPosition{ transform->GetWorldPosition() };
		const Vector3D nextPosition{ currentPosition + forward * (moveInput * moveSpeed * deltaTime) };
		transform->SetWorldPosition(nextPosition);
	}

	jumpRequested = false;
	dodgeRequested = false;
}

void PlayerController::OnInputStarted(const InputContext& context_)
{
	if (context_.IsKey(KeyCode::Space))
	{
		jumpRequested = true;
	}

	if (context_.IsKey(KeyCode::Shift))
	{
		dodgeRequested = true;
	}
}

void PlayerController::OnInputPerformed(const InputContext& context_)
{
	if (context_.IsKey(KeyCode::A) || context_.IsKey(KeyCode::Left))
	{
		yawInput = -1.0f;
	}
	else if (context_.IsKey(KeyCode::D) || context_.IsKey(KeyCode::Right))
	{
		yawInput = 1.0f;
	}
	else if (context_.IsKey(KeyCode::W) || context_.IsKey(KeyCode::Up))
	{
		moveInput = 1.0f;
	}
	else if (context_.IsKey(KeyCode::S) || context_.IsKey(KeyCode::Down))
	{
		moveInput = -1.0f;
	}
}

void PlayerController::OnInputCanceled(const InputContext& context_)
{
	if (context_.IsKey(KeyCode::A) || context_.IsKey(KeyCode::D) ||
		context_.IsKey(KeyCode::Left) || context_.IsKey(KeyCode::Right))
	{
		yawInput = 0.0f;
	}

	if (context_.IsKey(KeyCode::W) || context_.IsKey(KeyCode::S) ||
		context_.IsKey(KeyCode::Up) || context_.IsKey(KeyCode::Down))
	{
		moveInput = 0.0f;
	}
}
