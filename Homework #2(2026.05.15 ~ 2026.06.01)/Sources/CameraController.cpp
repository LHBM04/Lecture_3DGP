#include "Precompiled.h"
#include "CameraController.h"

#include "GameObject.h"
#include "Quaternion.h"
#include "Timer.h"
#include "Transform.h"
#include <limits>

void CameraController::OnUpdate()
{
	if (nullptr == target)
	{
		return;
	}

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

	const Quaternion targetRotation{ target->GetWorldRotation() };
	const Vector3D targetPosition{ target->GetWorldPosition() + (targetRotation * offset) };

	const Vector3D currentPosition{ transform->GetWorldPosition() };
	const Vector3D smoothedPosition{ Vector3D::SmoothDamp(
		currentPosition,
		targetPosition,
		followVelocity,
		followSmoothTime,
		std::numeric_limits<float>::infinity(),
		Timer::GetDeltaTime()) };

	transform->SetWorldPosition(smoothedPosition);

	const Vector3D toTarget{ target->GetWorldPosition() - smoothedPosition };
	if (toTarget.IsZero())
	{
		return;
	}

	transform->SetWorldRotation(Quaternion::LookRotation(toTarget, targetRotation * Vector3D::GetUp()));
}
