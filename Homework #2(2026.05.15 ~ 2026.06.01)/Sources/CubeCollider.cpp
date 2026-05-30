#include "Precompiled.h"
#include "CubeCollider.h"
#include "SphereCollider.h"
#include "Transform.h"
#include "GameObject.h"
#include "Vector3D.h"

void CubeCollider::SetCenter(const Vector3D& center_) noexcept
{
	localBox.Center = { center_.x, center_.y, center_.z };
}

void CubeCollider::SetSize(const Vector3D& size_) noexcept
{
	localBox.Extents = { size_.x * 0.5f, size_.y * 0.5f, size_.z * 0.5f };
}

Vector3D CubeCollider::GetSize() const noexcept
{
	return Vector3D{ localBox.Extents.x * 2.0f, localBox.Extents.y * 2.0f, localBox.Extents.z * 2.0f };
}

const DirectX::BoundingOrientedBox& CubeCollider::GetVolume() const noexcept
{
	return worldBox;
}

void CubeCollider::UpdateVolume()
{
	Transform* transform{ GetOwner()->GetComponent<Transform>() };
	if (transform == nullptr)
	{
		return;
	}

	localBox.Transform(worldBox, Matrix4x4::Load(transform->GetWorldMatrix()));
}

void CubeCollider::OnUpdate(float deltaTime_)
{
	UpdateVolume();
}

bool CubeCollider::IsIntersects(const Collider* other_) const
{
	return other_->IsIntersects(this);
}

bool CubeCollider::IsIntersects(const SphereCollider* other_) const
{
	return worldBox.Intersects(other_->GetVolume());
}

bool CubeCollider::IsIntersects(const CubeCollider* other_) const
{
	return worldBox.Intersects(other_->worldBox);
}

bool CubeCollider::IsIntersects(const Vector3D& rayOrigin_, const Vector3D& rayDir_, float& distance_) const
{
	return worldBox.Intersects(Vector3D::Load(rayOrigin_), Vector3D::Load(rayDir_), distance_);
}

bool CubeCollider::IsIntersects(const DirectX::BoundingFrustum& frustum_) const
{
	return frustum_.Intersects(worldBox);
}

DirectX::BoundingBox CubeCollider::GetBoundingVolume() const
{
	Vector3D corners[8];
	worldBox.GetCorners(corners);

	Vector3D min{ corners[0] };
	Vector3D max{ corners[0] };

	for (int i{ 1 }; i < 8; ++i)
	{
		min = Vector3D::Min(min, corners[i]);
		max = Vector3D::Max(max, corners[i]);
	}

	DirectX::BoundingBox box;
	DirectX::BoundingBox::CreateFromPoints(box, Vector3D::Load(min), Vector3D::Load(max));
	return box;
}
