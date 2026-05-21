#include "Precompiled.h"
#include "Camera.h"

#include "GameObject.h"
#include "CameraProjection_Perspective.h"
#include "Scene.h"
#include "Transform.h"

Camera::Camera()
	: projection(std::make_unique<CameraProjection_Perspective>())
{
}

CameraProjection* Camera::GetProjection() noexcept
{
	return projection.get();
}

const CameraProjection* Camera::GetProjection() const noexcept
{
	return projection.get();
}

void Camera::SetProjection(std::unique_ptr<CameraProjection> projection_) noexcept
{
	if (nullptr != projection_)
	{
		projection = std::move(projection_);
	}
}

float Camera::GetNearClipPlane() const noexcept
{
	return nearPlane;
}

void Camera::SetNearClipPlane(float nearClipPlane_) noexcept
{
	nearPlane = std::max(nearClipPlane_, Mathf::EPSILON);
	if (farPlane <= nearPlane)
	{
		farPlane = nearPlane + 1.0f;
	}
}

float Camera::GetFarClipPlane() const noexcept
{
	return farPlane;
}

void Camera::SetFarClipPlane(float farClipPlane_) noexcept
{
	farPlane = std::max(farClipPlane_, nearPlane + Mathf::EPSILON);
}

const Vector4D& Camera::GetViewportRect() const noexcept
{
	return viewportRect;
}

void Camera::SetViewportRect(const Vector4D& viewportRect_) noexcept
{
	viewportRect.x = Mathf::Clamp(viewportRect_.x, 0.0f, 1.0f);
	viewportRect.y = Mathf::Clamp(viewportRect_.y, 0.0f, 1.0f);
	viewportRect.z = Mathf::Clamp(viewportRect_.z, 0.0f, 1.0f - viewportRect.x);
	viewportRect.w = Mathf::Clamp(viewportRect_.w, 0.0f, 1.0f - viewportRect.y);
}

Matrix4x4 Camera::GetViewMatrix() const
{
	const Transform* transform{ GetOwner()->GetTransform()};
	if (nullptr == transform)
	{
		return Matrix4x4::GetIdentity();
	}

	return transform->GetWorldMatrix().GetInverse();
}

Matrix4x4 Camera::GetViewProjectionMatrix(float aspectRatio_) const
{
	return GetViewMatrix() * GetProjectionMatrix(aspectRatio_);
}

Matrix4x4 Camera::GetProjectionMatrix(float aspectRatio_) const
{
	return projection->GetProjectionMatrix(aspectRatio_, nearPlane, farPlane);
}

void Camera::OnAttach()
{
	GameObject* owner{ GetOwner() };
	if (nullptr != owner && nullptr != owner->GetCurrentScene())
	{
		owner->GetCurrentScene()->AddCamera(this);
	}
}

void Camera::OnDetach()
{
	GameObject* owner{ GetOwner() };
	if (nullptr != owner && nullptr != owner->GetCurrentScene())
	{
		owner->GetCurrentScene()->RemoveCamera(this);
	}
}
