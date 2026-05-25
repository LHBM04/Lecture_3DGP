#include "Precompiled.h"
#include "Camera.h"

#include "CameraProjection.h"
#include "CameraProjection_Perspective.h"
#include "CameraProjection_Orthographic.h"
#include "GameObject.h"
#include "Matrix4x4.h"
#include "Scene.h"
#include "Transform.h"
#include "Vector2D.h"
#include "Vector3D.h"

Camera::Camera() noexcept = default;
Camera::~Camera() noexcept = default;

CameraProjection* Camera::GetProjection() noexcept
{
	return projection.get();
}

const CameraProjection* Camera::GetProjection() const noexcept
{
	return projection.get();
}

void Camera::SetProjection(Projection projection_) noexcept
{
	projection.release();

	switch (projection_)
	{
	case Camera::Projection::Persprective:
	{
		projection = std::make_unique<CameraProjection_Perspective>();
		break;
	}
	case Camera::Projection::Orthographic:
	{
		projection = std::make_unique<CameraProjection_Orthographic>();
		break;
	}
	default:
	{
		break;
	}
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

const Vector4D& Camera::GetViewport() const noexcept
{
	return viewportRect;
}

void Camera::SetViewport(const Vector4D& viewport_) noexcept
{
	viewportRect.x = Mathf::Clamp(viewport_.x, 0.0f, 1.0f);
	viewportRect.y = Mathf::Clamp(viewport_.y, 0.0f, 1.0f);
	viewportRect.z = Mathf::Clamp(viewport_.z, 0.0f, 1.0f - viewportRect.x);
	viewportRect.w = Mathf::Clamp(viewport_.w, 0.0f, 1.0f - viewportRect.y);
}

CameraClearMode Camera::GetClearMode() const noexcept
{
	return clearMode;
}

void Camera::SetClearMode(CameraClearMode clearMode_) noexcept
{
	clearMode = clearMode_;
}

const ColorRGBA& Camera::GetClearColor() const noexcept
{
	return clearColor;
}

void Camera::SetClearColor(const ColorRGBA& clearColor_) noexcept
{
	clearColor = clearColor_;
}

Matrix4x4 Camera::GetViewMatrix() const
{
	const Transform* transform{ GetOwner()->GetComponent<Transform>() };
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
