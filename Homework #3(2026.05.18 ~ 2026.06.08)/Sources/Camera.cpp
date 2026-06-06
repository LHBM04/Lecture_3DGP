#include "Precompiled.h"
#include "Camera.h"

#include "CubeCollider.h"
#include "GameObject.h"
#include "Matrix4x4.h"
#include "RenderSystem.h"
#include "Scene.h"
#include "SphereCollider.h"
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
	const Transform* transform{ GetOwner()->GetComponent<Transform>() };

	if (transform == nullptr)
	{
		return Matrix4x4::GetIdentity();
	}

	return transform->GetWorldMatrix().GetInverse();
}

Matrix4x4 Camera::GetProjectionMatrix() const
{
	const Vector4D viewport{ GetViewportPixelRect() };
	
	float aspectRatio{ 1.0f };
	if (viewport.w > 0.0f)
	{
		aspectRatio = viewport.z / viewport.w;
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

Vector4D Camera::GetViewportPixelRect() const
{
	const RenderSystem& rs{ RenderSystem::GetInstance() };
	const D3D12_VIEWPORT& rsViewport{ rs.GetViewport() };

	const float screenWidth{ rsViewport.Width };
	const float screenHeight{ rsViewport.Height };

	return Vector4D(
		viewportRect.x * screenWidth,
		viewportRect.y * screenHeight,
		viewportRect.z * screenWidth,
		viewportRect.w * screenHeight);
}

const DirectX::BoundingFrustum& Camera::GetFrustum() const
{
	return frustum;
}

void Camera::OnEnable()
{
	GetOwner()->GetScene()->AddCamera(this);
}

void Camera::OnDisable()
{
	GetOwner()->GetScene()->RemoveCamera(this);
}

void Camera::UpdateFrustum()
{
	const Transform* transform{ GetOwner() != nullptr ? GetOwner()->GetComponent<Transform>() : nullptr };
	if (transform == nullptr)
	{
		return;
	}

	DirectX::BoundingFrustum::CreateFromMatrix(frustum, Matrix4x4::Load(GetProjectionMatrix()));
	frustum.Transform(frustum, Matrix4x4::Load(transform->GetWorldMatrix()));
}

bool Camera::IsInFrustum(const SphereCollider* collider_) const
{
	if (collider_ == nullptr) return true;
	return collider_->IsIntersects(frustum);
}

bool Camera::IsInFrustum(const CubeCollider* collider_) const
{
	if (collider_ == nullptr) return true;
	return collider_->IsIntersects(frustum);
}

void Camera::ScreenPointToRay(const Vector2D& screenPoint_, Vector3D& rayOrigin_, Vector3D& rayDir_) const
{
	const Vector4D viewport{ GetViewportPixelRect() };

	const float x{ (((screenPoint_.x - viewport.x) / viewport.z) * 2.0f) - 1.0f };
	const float y{ -((((screenPoint_.y - viewport.y) / viewport.w) * 2.0f) - 1.0f) };

	const Matrix4x4 invVP{ GetViewProjectionMatrix().GetInverse() };
	
	DirectX::XMVECTOR rayNear{ DirectX::XMVectorSet(x, y, 0.0f, 1.0f) };
	DirectX::XMVECTOR rayFar{ DirectX::XMVectorSet(x, y, 1.0f, 1.0f) };

	DirectX::XMVECTOR worldNear{ DirectX::XMVector3TransformCoord(rayNear, Matrix4x4::Load(invVP)) };
	DirectX::XMVECTOR worldFar{ DirectX::XMVector3TransformCoord(rayFar, Matrix4x4::Load(invVP)) };

	DirectX::XMVECTOR dir{ DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(worldFar, worldNear)) };

	rayOrigin_ = Vector3D(worldNear);
	rayDir_ = Vector3D(dir);
}
