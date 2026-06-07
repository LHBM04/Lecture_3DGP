#include "Precompiled.h"
#include "CameraController.h"

#include "GameObject.h"
#include "InputSystem.h"
#include "Quaternion.h"
#include "Scene.h"
#include "TimeSystem.h"
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

bool CameraController::IsFirstPerson() const noexcept
{
	return isFirstPerson;
}

void CameraController::SetFirstPerson(bool isFirstPerson_) noexcept
{
	if (isFirstPerson == isFirstPerson_)
	{
		return;
	}

	isFirstPerson = isFirstPerson_;
	followVelocity = Vector3D::GetZero();
}

const Vector3D& CameraController::GetFirstPersonOffset() const noexcept
{
	return firstPersonOffset;
}

void CameraController::SetFirstPersonOffset(const Vector3D& offset_) noexcept
{
	firstPersonOffset = offset_;
}

void CameraController::OnLateUpdate()
{
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

	if (target == nullptr)
	{
		return;
	}

	if (InputSystem::GetInstance().IsKeyPressed(KeyCode::Enter))
	{
		SetFirstPerson(!isFirstPerson);
	}

	const Quaternion targetRotation{ target->GetWorldRotation() };
	if (isFirstPerson)
	{
		const Vector3D targetPosition{ target->GetWorldPosition() + (targetRotation * firstPersonOffset) };
		transform->SetWorldPosition(targetPosition);
		// 1인칭에서는 타겟의 회전을 오차 없이 그대로 따라갑니다.
		transform->SetWorldRotation(targetRotation);
		return;
	}

	const Vector3D targetPosition{ target->GetWorldPosition() + (targetRotation * offset) };

	const Vector3D currentPosition{ transform->GetWorldPosition() };
	const Vector3D smoothedPosition{ Vector3D::SmoothDamp(
		currentPosition,
		targetPosition,
		followVelocity,
		followSmoothTime,
		std::numeric_limits<float>::infinity(),
		TimeSystem::GetInstance().GetDeltaTime()) };

	transform->SetWorldPosition(smoothedPosition);

	// 3인칭에서도 타겟의 회전을 기본으로 하되, 
	// 살짝 뒤쳐진 위치에서 타겟을 정확히 중앙에 놓기 위해 LookRotation을 보강합니다.
	const Vector3D toTarget{ target->GetWorldPosition() - transform->GetWorldPosition() };
	if (toTarget.GetSqrMagnitude() > Mathf::Epsilon)
	{
		// 타겟의 Up 방향을 유지하면서 타겟을 정면으로 바라봅니다.
		transform->SetWorldRotation(Quaternion::LookRotation(toTarget.GetNormalized(), targetRotation * Vector3D::GetUp()));
	}
	else
	{
		transform->SetWorldRotation(targetRotation);
	}
}
