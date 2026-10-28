#include "Precompiled.hpp"
#include "Camera.hpp"

#include "Renderer.hpp"
#include "Object.hpp"
#include "Scene.hpp"
#include "Transform.hpp"

Camera::Viewport Camera::GetViewport() const
{
	return viewport;
}

void Camera::SetViewport(const Viewport& viewport_)
{
	SetViewport(viewport_.x, viewport_.y, viewport_.width, viewport_.height);
}

void Camera::SetViewport(float x_, float y_, float width_, float height_)
{
	viewport.x = x_;
	viewport.y = y_;
	viewport.width = std::max(width_, Mathf::EPSILON);
	viewport.height = std::max(height_, Mathf::EPSILON);
}

float Camera::GetNearClipPlane() const
{
	return nearClipPlane;
}

void Camera::SetNearClipPlane(float nearClipPlane_)
{
	nearClipPlane = std::max(nearClipPlane_, Mathf::EPSILON);
	if (farClipPlane <= nearClipPlane)
	{
		farClipPlane = nearClipPlane + 1.0f;
	}
}

float Camera::GetFarClipPlane() const
{
	return farClipPlane;
}

void Camera::SetFarClipPlane(float farClipPlane_)
{
	farClipPlane = std::max(farClipPlane_, nearClipPlane + Mathf::EPSILON);
}

Matrix4x4 Camera::GetViewMatrix() const
{
	const Transform* transform = GetTransform();
	if (!transform)
	{
		return Matrix4x4::GetIdentity();
	}

	const Matrix4x4 worldMatrix = transform->GetWorldMatrix();
	Matrix4x4 viewMatrix;
	if (worldMatrix.TryGetInverse(viewMatrix))
	{
		return viewMatrix;
	}

	return Matrix4x4::GetIdentity();
}

Matrix4x4 Camera::GetViewProjectionMatrix(float aspectRatio_) const
{
	return GetViewMatrix() * GetProjectionMatrix(aspectRatio_);
}

void Camera::OnAttach()
{
	GetOwner()->GetCurrentScene()->AddCamera(this);
}

void Camera::OnDetach()
{
	GetOwner()->GetCurrentScene()->RemoveCamera(this);
}
