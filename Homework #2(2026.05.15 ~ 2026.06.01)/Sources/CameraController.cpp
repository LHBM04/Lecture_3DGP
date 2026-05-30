#include "Precompiled.h"
#include "CameraController.h"

#include "GameObject.h"
#include "Quaternion.h"
#include "Transform.h"

Transform* CameraController::GetTarget() const noexcept
{
	return target;
}

void CameraController::SetTarget(Transform* target_) noexcept
{
	target = target_;
}

const Vector3D& CameraController::GetOffset() const noexcept 
{
	return offset;
}

void CameraController::SetOffset(const Vector3D& offset_) noexcept
{
	offset = offset_;
}

const Vector3D& CameraController::GetFollowVelocity() const noexcept
{
	return followVelocity;
}

void CameraController::SetFollowVelocity(const Vector3D& velocity_) noexcept
{
	followVelocity = velocity_;
}

float CameraController::GetSmoothTime() const noexcept
{
	return followSmoothTime;
}

void CameraController::SetSmoothTime(float smoothTime_) noexcept
{
	followSmoothTime = smoothTime_;
}

void CameraController::OnLateUpdate(float deltaTime_)
{
	if (target == nullptr)
	{
		return;
	}

	GameObject* owner{ GetOwner() };
	if (owner == nullptr)
	{
		return;
	}

	Transform* transform{ owner->GetComponent<Transform>() };
	if (transform == nullptr)
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
		deltaTime_) };

	transform->SetWorldPosition(smoothedPosition);

	const Vector3D toTarget{ target->GetWorldPosition() - smoothedPosition };
	if (toTarget.IsZero())
	{
		return;
	}

	transform->SetWorldRotation(Quaternion::LookRotation(toTarget, targetRotation * Vector3D::GetUp()));
}
