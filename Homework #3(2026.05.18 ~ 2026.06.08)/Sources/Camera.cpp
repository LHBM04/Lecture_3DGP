#include "Precompiled.h"
#include "Camera.h"

#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"

float Camera::GetNearClipPlane() const noexcept
{
	return nearClipPlane;
}

void Camera::SetNearClipPlane(float nearClipPlane_) noexcept
{
	nearClipPlane = std::max(0.001f, nearClipPlane_);
}

float Camera::GetFarClipPlane() const noexcept
{
	return farClipPlane;
}

void Camera::SetFarClipPlane(float farClipPlane_) noexcept
{
	farClipPlane = std::max(nearClipPlane + 0.001f, farClipPlane_);
}

const Vector4D& Camera::GetViewport() const noexcept
{
	return viewportRect;
}

void Camera::SetViewport(const Vector4D& viewport_) noexcept
{
	viewportRect = viewport_;
}

Camera::ProjectionType Camera::GetProjectionType() const noexcept
{
	return projectionType;
}

void Camera::SetProjectionType(ProjectionType projectionType_) noexcept
{
	projectionType = projectionType_;
}

float Camera::GetFieldOfView() const noexcept
{
	return fieldOfViewDegrees;
}

void Camera::SetFieldOfView(float fieldOfViewDegrees_) noexcept
{
	fieldOfViewDegrees = std::max(1.0f, fieldOfViewDegrees_);
}

float Camera::GetOrthographicSize() const noexcept
{
	return orthographicSize;
}

void Camera::SetOrthographicSize(float orthographicSize_) noexcept
{
	orthographicSize = std::max(0.001f, orthographicSize_);
}

float Camera::GetAspectRatio() const noexcept
{
	return aspectRatio;
}

void Camera::SetAspectRatio(float aspectRatio_) noexcept
{
	aspectRatio = std::max(0.001f, aspectRatio_);
}

Camera::ClearType Camera::GetClearMode() const noexcept
{
	return clearType;
}

void Camera::SetClearMode(ClearType clearMode_) noexcept
{
	clearType = clearMode_;
}

const ColorRGBA& Camera::GetClearColor() const noexcept
{
	return clearColor;
}

void Camera::SetClearColor(const ColorRGBA& clearColor_) noexcept
{
	clearColor = clearColor_;
}

Matrix4x4 Camera::GetViewMatrix() const noexcept
{
	const GameObject* const owner{ GetOwner() };
	const Transform* const transform{ (owner != nullptr) ? owner->GetTransform() : nullptr };
	if (transform == nullptr)
	{
		return Matrix4x4::GetIdentity();
	}

	return transform->GetWorldMatrix().GetInverse();
}

Matrix4x4 Camera::GetProjectionMatrix() const noexcept
{
	if (projectionType == ProjectionType::Orthographic)
	{
		const float halfHeight{ orthographicSize };
		const float halfWidth{ halfHeight * aspectRatio };
		return Matrix4x4::Ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, nearClipPlane, farClipPlane);
	}

	return Matrix4x4::Perspective(fieldOfViewDegrees, aspectRatio, nearClipPlane, farClipPlane);
}

Matrix4x4 Camera::GetViewProjectionMatrix() const noexcept
{
	return GetViewMatrix() * GetProjectionMatrix();
}

void Camera::OnEnable()
{
	if (GameObject* const owner{ GetOwner() }; owner != nullptr && owner->GetScene() != nullptr)
	{
		owner->GetScene()->AddCamera(this);
	}
}

void Camera::OnDisable()
{
	if (GameObject* const owner{ GetOwner() }; owner != nullptr && owner->GetScene() != nullptr)
	{
		owner->GetScene()->RemoveCamera(this);
	}
}
