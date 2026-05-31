#include "Precompiled.h"
#include "StairCollider.h"

#include "CubeCollider.h"
#include "GameObject.h"
#include "SphereCollider.h"
#include "Transform.h"
#include "Vector3D.h"

void StairCollider::SetCenter(const Vector3D& center_) noexcept
{
	localCenter = center_;
}

void StairCollider::SetSize(const Vector3D& size_) noexcept
{
	localSize = size_;
}

void StairCollider::SetSlopeAxis(SlopeAxis axis_) noexcept
{
	slopeAxis = axis_;
}

bool StairCollider::IsIntersects(const Collider* other_) const
{
	if (const SphereCollider* sphere{ dynamic_cast<const SphereCollider*>(other_) })
	{
		return IsIntersects(sphere);
	}
	if (const CubeCollider* cube{ dynamic_cast<const CubeCollider*>(other_) })
	{
		return IsIntersects(cube);
	}
	return false;
}

bool StairCollider::IsIntersects(const SphereCollider* other_) const
{
	return worldBox.Intersects(other_->GetVolume());
}

bool StairCollider::IsIntersects(const CubeCollider* other_) const
{
	if (!worldBox.Intersects(other_->GetVolume()))
	{
		return false;
	}

	Transform* transform{ GetOwner()->GetComponent<Transform>() };
	if (transform == nullptr)
	{
		return false;
	}

	const Matrix4x4 invWorld{ transform->GetWorldMatrix().GetInverse() };
	Vector3D cubeCorners[8];
	other_->GetVolume().GetCorners(cubeCorners);

	for (const Vector3D& corner : cubeCorners)
	{
		const Vector3D localPoint{ invWorld.MultiplyPoint(corner) };
		if (IsPointInsideWedgeLocal(localPoint))
		{
			return true;
		}
	}

	const Vector3D half{ localSize * 0.5f };
	const Vector3D min{ localCenter - half };
	const Vector3D max{ localCenter + half };
	Vector3D candidates[8]{
		Vector3D(min.x, min.y, min.z), Vector3D(max.x, min.y, min.z), Vector3D(min.x, min.y, max.z), Vector3D(max.x, min.y, max.z),
		Vector3D(min.x, max.y, min.z), Vector3D(max.x, max.y, min.z), Vector3D(min.x, max.y, max.z), Vector3D(max.x, max.y, max.z),
	};

	const Matrix4x4 world{ transform->GetWorldMatrix() };
	for (const Vector3D& candidate : candidates)
	{
		if (!IsPointInsideWedgeLocal(candidate))
		{
			continue;
		}

		const Vector3D worldPoint{ world.MultiplyPoint(candidate) };
		if (other_->GetVolume().Contains(Vector3D::Load(worldPoint)) != DirectX::ContainmentType::DISJOINT)
		{
			return true;
		}
	}

	return false;
}

bool StairCollider::IsIntersects(const Vector3D& rayOrigin_, const Vector3D& rayDir_, float& distance_) const
{
	return worldBox.Intersects(Vector3D::Load(rayOrigin_), Vector3D::Load(rayDir_), distance_);
}

bool StairCollider::IsIntersects(const DirectX::BoundingFrustum& frustum_) const
{
	return frustum_.Intersects(worldBox);
}

DirectX::BoundingBox StairCollider::GetBoundingVolume() const
{
	Vector3D corners[8];
	worldBox.GetCorners(corners);

	Vector3D min = corners[0];
	Vector3D max = corners[0];
	for (int i = 1; i < 8; ++i)
	{
		min = Vector3D::Min(min, corners[i]);
		max = Vector3D::Max(max, corners[i]);
	}

	DirectX::BoundingBox box;
	DirectX::BoundingBox::CreateFromPoints(box, Vector3D::Load(min), Vector3D::Load(max));
	return box;
}

void StairCollider::UpdateVolume()
{
	Transform* transform{ GetOwner()->GetComponent<Transform>() };
	if (transform == nullptr)
	{
		return;
	}

	DirectX::BoundingOrientedBox localBox{
		{ localCenter.x, localCenter.y, localCenter.z },
		{ localSize.x * 0.5f, localSize.y * 0.5f, localSize.z * 0.5f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};

	localBox.Transform(worldBox, Matrix4x4::Load(transform->GetWorldMatrix()));
}

void StairCollider::OnUpdate(float deltaTime_)
{
	UpdateVolume();
}

bool StairCollider::IsPointInsideWedgeLocal(const Vector3D& pointLocal_) const
{
	const float epsilon{ 0.0001f };

	const Vector3D half{ localSize * 0.5f };
	const Vector3D min{ localCenter - half };
	const Vector3D max{ localCenter + half };

	if (pointLocal_.x < min.x - epsilon || pointLocal_.x > max.x + epsilon)
	{
		return false;
	}
	if (pointLocal_.z < min.z - epsilon || pointLocal_.z > max.z + epsilon)
	{
		return false;
	}
	if (pointLocal_.y < min.y - epsilon || pointLocal_.y > max.y + epsilon)
	{
		return false;
	}

	float t{ 0.0f };
	switch (slopeAxis)
	{
	case SlopeAxis::PositiveZ:
		t = (pointLocal_.z - min.z) / std::max(max.z - min.z, epsilon);
		break;
	case SlopeAxis::NegativeZ:
		t = (max.z - pointLocal_.z) / std::max(max.z - min.z, epsilon);
		break;
	case SlopeAxis::PositiveX:
		t = (pointLocal_.x - min.x) / std::max(max.x - min.x, epsilon);
		break;
	case SlopeAxis::NegativeX:
		t = (max.x - pointLocal_.x) / std::max(max.x - min.x, epsilon);
		break;
	default:
		return false;
	}

	const float topY{ min.y + t * (max.y - min.y) };
	return pointLocal_.y <= topY + epsilon;
}
