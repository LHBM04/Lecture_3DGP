#include "Precompiled.h"
#include "CameraController.h"

#include "GameObject.h"
#include "InputSystem.h"
#include "Quaternion.h"
#include "Scene.h"
#include "Transform.h"

Transform* CameraController::ResolveTarget()
{
	GameObject* owner{ GetOwner() };
	if (owner == nullptr)
	{
		return nullptr;
	}

	Scene* scene{ owner->GetScene() };
	if (scene == nullptr)
	{
		return nullptr;
	}

	GameObject* targetObject{ nullptr };
	if (useTargetTag)
	{
		targetObject = scene->FindObjectWithTag(targetTag);
	}
	else
	{
		targetObject = scene->FindObjectWithName(targetName);
	}

	if (targetObject == nullptr || targetObject->IsDestroyed())
	{
		return nullptr;
	}

	return targetObject->GetComponent<Transform>();
}

Transform* CameraController::GetTarget() const noexcept
{
	return const_cast<CameraController*>(this)->ResolveTarget();
}

void CameraController::SetTarget(Transform* target_) noexcept
{
	targetTag.clear();
	targetName.clear();
	useTargetTag = false;

	if (target_ == nullptr)
	{
		return;
	}

	GameObject* targetOwner{ target_->GetOwner() };
	if (targetOwner == nullptr)
	{
		return;
	}

	const std::wstring_view tag{ targetOwner->GetTag() };
	if (!tag.empty() && tag != L"Untagged")
	{
		targetTag.assign(tag);
		useTargetTag = true;
		return;
	}

	targetName.assign(targetOwner->GetName());
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

void CameraController::SetFirstPersonOffset(const Vector3D& offset_) noexcept
{
	firstPersonOffset = offset_;
}

void CameraController::SetThirdPersonOffset(const Vector3D& offset_) noexcept
{
	thirdPersonOffset = offset_;
	offset = offset_;
}

void CameraController::SetCrosshairObject(GameObject* crosshair_) noexcept
{
	crosshairObject = crosshair_;
	if (crosshairObject != nullptr)
	{
		crosshairObject->SetActive(isFirstPerson);
	}
}

void CameraController::OnLateUpdate(float deltaTime_)
{
	Transform* target{ ResolveTarget() };
	if (target == nullptr)
	{
		if (crosshairObject != nullptr)
		{
			crosshairObject->SetActive(false);
		}
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

	if (InputSystem::GetInstance().IsKeyPressed(KeyCode::Enter))
	{
		isFirstPerson = !isFirstPerson;
		offset = isFirstPerson ? firstPersonOffset : thirdPersonOffset;
		if (crosshairObject != nullptr)
		{
			crosshairObject->SetActive(isFirstPerson);
		}
	}

	const Quaternion targetRotation{ target->GetWorldRotation() };
	const Vector3D targetPosition{ target->GetWorldPosition() + (targetRotation * offset) };

	if (isFirstPerson)
	{
		transform->SetWorldPosition(targetPosition);
		transform->SetWorldRotation(targetRotation);
		return;
	}

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
