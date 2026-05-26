#include "Precompiled.h"
#include "CubeCollider.h"

#include "GameObject.h"
#include "Quaternion.h"
#include "Transform.h"

const Vector3D& CubeCollider::GetCenter() const noexcept
{
	return center;
}

void CubeCollider::SetCenter(const Vector3D& center_) noexcept
{
	center = center_;
}

const Vector3D& CubeCollider::GetSize() const noexcept
{
	return size;
}

void CubeCollider::SetSize(const Vector3D& size_) noexcept
{
	size.x = std::max(size_.x, 0.0f);
	size.y = std::max(size_.y, 0.0f);
	size.z = std::max(size_.z, 0.0f);
}

bool CubeCollider::Intersects(const DirectX::BoundingFrustum& frustum_) const
{
	return frustum_.Intersects(BuildWorldOrientedBox());
}

bool CubeCollider::Intersects(const Collider& other_) const
{
	const CubeCollider* otherBox{ dynamic_cast<const CubeCollider*>(&other_) };
	if (nullptr == otherBox)
	{
		return false;
	}

	return BuildWorldOrientedBox().Intersects(otherBox->BuildWorldOrientedBox());
}

DirectX::BoundingOrientedBox CubeCollider::GetWorldOrientedBox() const
{
	return BuildWorldOrientedBox();
}

DirectX::BoundingOrientedBox CubeCollider::BuildWorldOrientedBox() const
{
	DirectX::BoundingOrientedBox box{};

	const GameObject* owner{ GetOwner() };
	if (nullptr == owner)
	{
		return box;
	}

	const Transform* transform{ owner->GetComponent<Transform>() };
	if (nullptr == transform)
	{
		return box;
	}

	const Matrix4x4 world{ transform->GetWorldMatrix() };
	const Vector3D worldCenter{ world.MultiplyPoint(center) };

	const Vector3D worldScaleRaw{ transform->GetWorldScale() };
	const Vector3D worldScale{
		std::abs(worldScaleRaw.x),
		std::abs(worldScaleRaw.y),
		std::abs(worldScaleRaw.z)
	};

	const Vector3D halfExtents{
		size.x * worldScale.x * 0.5f,
		size.y * worldScale.y * 0.5f,
		size.z * worldScale.z * 0.5f
	};

	const Quaternion worldRotation{
		Quaternion::Normalize(transform->GetWorldRotation())
	};

	box.Center = DirectX::XMFLOAT3(worldCenter.x, worldCenter.y, worldCenter.z);
	box.Extents = DirectX::XMFLOAT3(halfExtents.x, halfExtents.y, halfExtents.z);
	box.Orientation = DirectX::XMFLOAT4(worldRotation.x, worldRotation.y, worldRotation.z, worldRotation.w);
	return box;
}
