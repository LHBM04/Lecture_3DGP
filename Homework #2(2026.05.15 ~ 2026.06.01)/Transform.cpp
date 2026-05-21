#include "Precompiled.h"
#include "Transform.h"

const Vector3D& Transform::GetLocalPosition() const
{
	return position;
}

void Transform::SetLocalPosition(const Vector3D& position_)
{
	position = position_;
}

const Quaternion& Transform::GetLocalRotation() const
{
	return rotation;
}

void Transform::SetLocalRotation(const Quaternion& rotation_)
{
	rotation = rotation_;
}

const Vector3D& Transform::GetLocalScale() const
{
	return scale;
}

void Transform::SetLocalScale(const Vector3D& scale_)
{
	scale = scale_;
}

Vector3D Transform::GetWorldPosition() const
{
	return GetWorldMatrix().GetWorldPosition();
}

void Transform::SetWorldPosition(const Vector3D& position_)
{
	if (nullptr == parent)
	{
		position = position_;
		return;
	}

	position = parent->GetWorldMatrix().GetInverse().MultiplyPoint(position_);
}

Quaternion Transform::GetWorldRotation() const
{
	if (nullptr == parent)
	{
		return rotation;
	}

	return parent->GetWorldRotation() * rotation;
}

void Transform::SetWorldRotation(const Quaternion& rotation_)
{
	if (nullptr == parent)
	{
		rotation = rotation_;
		return;
	}

	rotation = Quaternion::Inverse(parent->GetWorldRotation()) * rotation_;
}

Vector3D Transform::GetWorldScale() const
{
	return GetWorldMatrix().GetScale();
}

void Transform::SetWorldScale(const Vector3D& scale_)
{
	if (nullptr == parent)
	{
		scale = scale_;
		return;
	}

	const Vector3D parentScale{ parent->GetWorldScale() };
	scale = Vector3D(
		parentScale.x != 0.0f ? scale_.x / parentScale.x : scale_.x,
		parentScale.y != 0.0f ? scale_.y / parentScale.y : scale_.y,
		parentScale.z != 0.0f ? scale_.z / parentScale.z : scale_.z);
}

Transform* const Transform::GetParent()
{
	return parent;
}

Transform* const Transform::GetParent() const
{
	return parent;
}

void Transform::SetParent(Transform* parent_)
{
	parent = parent_;
}

Matrix4x4 Transform::GetLocalMatrix() const
{
	return Matrix4x4::TRS(position, rotation, scale);
}

Matrix4x4 Transform::GetWorldMatrix() const
{
	if (nullptr == parent)
	{
		return GetLocalMatrix();
	}

	return GetLocalMatrix() * parent->GetWorldMatrix();
}
