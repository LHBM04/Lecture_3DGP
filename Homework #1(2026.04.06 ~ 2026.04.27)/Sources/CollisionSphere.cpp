#include "Precompiled.hpp"
#include "CollisionSphere.hpp"

#include "Transform.hpp"

const Vector3D& CollisionSphere::GetLocalCenter() const
{
	return localCenter;
}

void CollisionSphere::SetLocalCenter(const Vector3D& localCenter_)
{
	localCenter = localCenter_;
	UpdateBounds();
}

float CollisionSphere::GetLocalRadius() const
{
	return localRadius;
}

void CollisionSphere::SetLocalRadius(float localRadius_)
{
	localRadius = std::max(localRadius_, Mathf::EPSILON);
	UpdateBounds();
}

const DirectX::BoundingSphere& CollisionSphere::GetBounds() const
{
	return bounds;
}

DirectX::BoundingBox CollisionSphere::GetWorldAABB() const
{
	DirectX::BoundingBox aabb{};
	DirectX::BoundingBox::CreateFromSphere(aabb, bounds);
	return aabb;
}

void CollisionSphere::UpdateBounds()
{
	const Transform* transform = GetTransform();
	if (!transform)
	{
		bounds.Center = DirectX::XMFLOAT3(localCenter.x, localCenter.y, localCenter.z);
		bounds.Radius = std::max(localRadius, Mathf::EPSILON);
		return;
	}

	const Matrix4x4 worldMatrix = transform->GetWorldMatrix();
	const Vector3D worldCenter = worldMatrix.MultiplyPoint(localCenter);
	const Vector3D worldScale = transform->GetWorldScale();
	const float maxScale = std::max({ std::abs(worldScale.x), std::abs(worldScale.y), std::abs(worldScale.z), 1.0f });

	bounds.Center = DirectX::XMFLOAT3(worldCenter.x, worldCenter.y, worldCenter.z);
	bounds.Radius = std::max(localRadius * maxScale, Mathf::EPSILON);
}
