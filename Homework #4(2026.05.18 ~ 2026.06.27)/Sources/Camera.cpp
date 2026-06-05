#include "Precompiled.h"
#include "Camera.h"

#include "GameObject.h"
#include "Matrix4x4.h"
#include "RenderContext.h"
#include "Scene.h"
#include "Transform.h"
#include "Vector2D.h"
#include "Vector3D.h"

float Camera::GetNearClipPlane() const
{
	return nearPlane;
}

void Camera::SetNearClipPlane(float nearClipPlane_)
{
	nearPlane = nearClipPlane_;
}

float Camera::GetFarClipPlane() const
{
	return farPlane;
}

void Camera::SetFarClipPlane(float farClipPlane_)
{
	farPlane = farClipPlane_;
}

const Vector4D& Camera::GetViewport() const
{
	return viewportRect;
}

void Camera::SetViewport(const Vector4D& viewport_)
{
	viewportRect = viewport_;
}

Camera::ProjectionType Camera::GetProjectionType() const
{
	return projectionType;
}

void Camera::SetProjectionType(ProjectionType projectionType_)
{
	projectionType = projectionType_;
}

float Camera::GetFOV() const
{
	return fov;
}

void Camera::SetFOV(float fov_)
{
	fov = fov_;
}

float Camera::GetOrthographicSize() const
{
	return orthographicSize;
}

void Camera::SetOrthographicSize(float orthographicSize_)
{
	orthographicSize = orthographicSize_;
}

Camera::ClearType Camera::GetClearMode() const
{
	return clearType;
}

void Camera::SetClearMode(ClearType clearMode_)
{
	clearType = clearMode_;
}

const ColorRGBA& Camera::GetClearColor() const
{
	return clearColor;
}

void Camera::SetClearColor(const ColorRGBA& clearColor_)
{
	clearColor = clearColor_;
}

Matrix4x4 Camera::GetViewMatrix() const
{
	return GetOwner().GetTransform().GetWorldMatrix().GetInverse();
}

Matrix4x4 Camera::GetProjectionMatrix() const
{
	float aspectRatio{ 1.0f };
	if (viewportRect.w > 0.0f)
	{
		aspectRatio = viewportRect.z / viewportRect.w;
	}

	switch (projectionType)
	{
		case ProjectionType::Perspective:
		{
			return Matrix4x4::Perspective(fov, aspectRatio, nearPlane, farPlane);
		}
		case ProjectionType::Orthographic:
		{
			const float halfHeight{ orthographicSize };
			const float halfWidth{ halfHeight * aspectRatio };

			return Matrix4x4::Ortho(
				-halfWidth, halfWidth,
				-halfHeight, halfHeight,
				GetNearClipPlane(), GetFarClipPlane());
		}
	}

	std::unreachable();
}

Matrix4x4 Camera::GetViewProjectionMatrix() const
{
	return GetViewMatrix() * GetProjectionMatrix();
}

const DirectX::BoundingFrustum& Camera::GetFrustum() const
{
	return frustum;
}

bool Camera::IsInFrustum(const Collider& collider_) const
{
	return true;
}

void Camera::OnEnable()
{
	GetOwner().GetCurrentScene().AddCamera(*this);
}

void Camera::OnDisable()
{
	GetOwner().GetCurrentScene().RemoveCamera(*this);
}

void Camera::UpdateFrustum()
{
	DirectX::BoundingFrustum::CreateFromMatrix(frustum, Matrix4x4::Load(GetProjectionMatrix()));
	frustum.Transform(frustum, Matrix4x4::Load(GetOwner().GetTransform().GetWorldMatrix()));
}

