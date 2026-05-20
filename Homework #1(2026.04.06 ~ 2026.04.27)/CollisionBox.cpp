#include "Precompiled.hpp"
#include "CollisionBox.hpp"

#include "Transform.hpp"

const Vector3D& CollisionBox::GetLocalCenter() const
{
	return localCenter;
}

void CollisionBox::SetLocalCenter(const Vector3D& localCenter_)
{
	localCenter = localCenter_;
	UpdateBounds();
}

const Vector3D& CollisionBox::GetLocalExtents() const
{
	return localExtents;
}

void CollisionBox::SetLocalExtents(const Vector3D& localExtents_)
{
	localExtents.x = std::max(std::abs(localExtents_.x), Mathf::EPSILON);
	localExtents.y = std::max(std::abs(localExtents_.y), Mathf::EPSILON);
	localExtents.z = std::max(std::abs(localExtents_.z), Mathf::EPSILON);
	UpdateBounds();
}

const DirectX::BoundingBox& CollisionBox::GetBounds() const
{
	return bounds;
}

DirectX::BoundingBox CollisionBox::GetWorldAABB() const
{
	return bounds;
}

void CollisionBox::UpdateBounds()
{
	const Transform* transform = GetTransform();
	if (!transform)
	{
		bounds.Center = DirectX::XMFLOAT3(localCenter.x, localCenter.y, localCenter.z);
		bounds.Extents = DirectX::XMFLOAT3(localExtents.x, localExtents.y, localExtents.z);
		return;
	}

	const Matrix4x4 worldMatrix = transform->GetWorldMatrix();

	Vector3D minPoint = Vector3D::GetPositiveInfinity();
	Vector3D maxPoint = Vector3D::GetNegativeInfinity();

	for (int xSign = -1; xSign <= 1; xSign += 2)
	{
		for (int ySign = -1; ySign <= 1; ySign += 2)
		{
			for (int zSign = -1; zSign <= 1; zSign += 2)
			{
				const Vector3D localCorner(
					localCenter.x + static_cast<float>(xSign) * localExtents.x,
					localCenter.y + static_cast<float>(ySign) * localExtents.y,
					localCenter.z + static_cast<float>(zSign) * localExtents.z);
				const Vector3D worldCorner = worldMatrix.MultiplyPoint(localCorner);

				minPoint = Vector3D::Min(minPoint, worldCorner);
				maxPoint = Vector3D::Max(maxPoint, worldCorner);
			}
		}
	}

	const Vector3D center = (minPoint + maxPoint) * 0.5f;
	const Vector3D extents = (maxPoint - minPoint) * 0.5f;

	bounds.Center = DirectX::XMFLOAT3(center.x, center.y, center.z);
	bounds.Extents = DirectX::XMFLOAT3(
		std::max(std::abs(extents.x), Mathf::EPSILON),
		std::max(std::abs(extents.y), Mathf::EPSILON),
		std::max(std::abs(extents.z), Mathf::EPSILON));
}
