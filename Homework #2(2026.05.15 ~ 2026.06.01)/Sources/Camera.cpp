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
	const RenderSystem& rs{ RenderSystem::GetInstance() };
	const float width{ rs.GetViewport().Width };
	const float height{ rs.GetViewport().Height };
	
	float aspectRatio{ 1.0f };
	if (height > 0.0f && viewportRect.w > 0.0f)
	{
		const float screenAspect{ width / height };
		aspectRatio = (viewportRect.z / viewportRect.w) * screenAspect;
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
	const RenderSystem& rs{ RenderSystem::GetInstance() };
	const D3D12_VIEWPORT& rsViewport{ rs.GetViewport() };
	
	const float sw{ rsViewport.Width };
	const float sh{ rsViewport.Height };

	const float vx{ viewportRect.x * sw }; 
	const float vy{ viewportRect.y * sh };
	const float vw{ viewportRect.z * sw };
	const float vh{ viewportRect.w * sh };

	const float x{ (((screenPoint_.x - vx) / vw) * 2.0f) - 1.0f };
	const float y{ -((((screenPoint_.y - vy) / vh) * 2.0f) - 1.0f) };

	const Matrix4x4 invVP{ GetViewProjectionMatrix().GetInverse() };
	
	DirectX::XMVECTOR rayNear{ DirectX::XMVectorSet(x, y, 0.0f, 1.0f) };
	DirectX::XMVECTOR rayFar{ DirectX::XMVectorSet(x, y, 1.0f, 1.0f) };

	DirectX::XMVECTOR worldNear{ DirectX::XMVector3TransformCoord(rayNear, Matrix4x4::Load(invVP)) };
	DirectX::XMVECTOR worldFar{ DirectX::XMVector3TransformCoord(rayFar, Matrix4x4::Load(invVP)) };

	DirectX::XMVECTOR dir{ DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(worldFar, worldNear)) };

	rayOrigin_ = Vector3D(worldNear);
	rayDir_ = Vector3D(dir);
}
