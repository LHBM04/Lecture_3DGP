#include "Precompiled.hpp"
#include "CameraController.hpp"

#include "Quaternion.hpp"
#include "Transform.hpp"
#include "Timer.hpp"

Transform* CameraController::GetTarget() const
{
	return target;
}

void CameraController::SetTarget(Transform* target_)
{
	target = target_;
	followVelocity = Vector3D::GetZero();
}

const Vector3D& CameraController::GetOffset() const
{
	return offset;
}

void CameraController::SetOffset(const Vector3D& offset_)
{
	offset = offset_;
}

void CameraController::OnLateUpdate()
{
	if (!target)
	{
		return;
	}

	Transform* transform = GetTransform();
	if (!transform)
	{
		return;
	}

	const Quaternion targetRotation = target->GetWorldRotation();
	const Vector3D targetPosition = target->GetWorldPosition() + (targetRotation * offset);

	const Vector3D currentPosition = transform->GetWorldPosition();
	const Vector3D smoothedPosition = Vector3D::SmoothDamp(
		currentPosition,
		targetPosition,
		followVelocity,
		0.125f,
		std::numeric_limits<float>::infinity(),
		Timer::GetDeltaTime()
	);

	transform->SetWorldPosition(smoothedPosition);

	const Vector3D toTarget = target->GetWorldPosition() - smoothedPosition;
	if (toTarget.IsZero())
	{
		return;
	}

	transform->SetWorldRotation(Quaternion::LookRotation(toTarget, targetRotation * Vector3D::GetUp()));
}
