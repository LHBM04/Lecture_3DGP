#include "Precompiled.h"
#include "SphereCollider.h"
#include "CubeCollider.h"
#include "Transform.h"
#include "GameObject.h"

float SphereCollider::GetRadius() const noexcept
{
	return localSphere.Radius;
}

void SphereCollider::SetRadius(float radius_) noexcept
{
	localSphere.Radius = radius_;
}

const DirectX::BoundingSphere& SphereCollider::GetVolume() const noexcept
{
	return worldSphere;
}

void SphereCollider::UpdateVolume()
{
	Transform* transform{ GetOwner()->GetComponent<Transform>() };
	if (transform == nullptr)
	{
		return;
	}

	localSphere.Transform(worldSphere, Matrix4x4::Load(transform->GetWorldMatrix()));
}

void SphereCollider::OnUpdate(float deltaTime_)
{
	UpdateVolume();
}

bool SphereCollider::IsIntersects(const Collider* other_) const
{
	// First dispatch: call the other's virtual IsIntersects with *this (SphereCollider)
	return other_->IsIntersects(this);
}

bool SphereCollider::IsIntersects(const SphereCollider* other_) const
{
	return worldSphere.Intersects(other_->worldSphere);
}

bool SphereCollider::IsIntersects(const CubeCollider* other_) const
{
	return worldSphere.Intersects(other_->GetVolume());
}

bool SphereCollider::IsIntersects(const Vector3D& rayOrigin_, const Vector3D& rayDir_, float& distance_) const
{
	return worldSphere.Intersects(Vector3D::Load(rayOrigin_), Vector3D::Load(rayDir_), distance_);
}

bool SphereCollider::IsIntersects(const DirectX::BoundingFrustum& frustum_) const
{
	return frustum_.Intersects(worldSphere);
}

DirectX::BoundingBox SphereCollider::GetBoundingVolume() const
{
	DirectX::BoundingBox box;
	DirectX::BoundingBox::CreateFromSphere(box, worldSphere);
	return box;
}
