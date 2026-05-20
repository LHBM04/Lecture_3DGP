#include "Precompiled.hpp"
#include "Transform.hpp"

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
	rotation = Quaternion::Normalize(rotation_);
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
	if (!parent)
	{
		position = position_;
		return;
	}

	Matrix4x4 parentWorldInverse;
	if (parent->GetWorldMatrix().TryGetInverse(parentWorldInverse))
	{
		position = parentWorldInverse.MultiplyPoint(position_);
		return;
	}

	position = position_;
}

Quaternion Transform::GetWorldRotation() const
{
	return GetWorldMatrix().GetRotation();
}

void Transform::SetWorldRotation(const Quaternion& rotation_)
{
	const Quaternion normalizedRotation = Quaternion::Normalize(rotation_);

	if (!parent)
	{
		rotation = normalizedRotation;
		return;
	}

	const Quaternion parentWorldRotation = parent->GetWorldRotation();
	rotation = Quaternion::Normalize(normalizedRotation * Quaternion::Inverse(parentWorldRotation));
}

Vector3D Transform::GetWorldScale() const
{
	return GetWorldMatrix().GetScale();
}

void Transform::SetWorldScale(const Vector3D& scale_)
{
	if (!parent)
	{
		scale = scale_;
		return;
	}

	const Vector3D parentWorldScale = parent->GetWorldScale();
	scale.x = std::abs(parentWorldScale.x) > Mathf::EPSILON ? (scale_.x / parentWorldScale.x) : scale_.x;
	scale.y = std::abs(parentWorldScale.y) > Mathf::EPSILON ? (scale_.y / parentWorldScale.y) : scale_.y;
	scale.z = std::abs(parentWorldScale.z) > Mathf::EPSILON ? (scale_.z / parentWorldScale.z) : scale_.z;
}

Transform* Transform::GetParent()
{
    return parent;
}

const Transform* Transform::GetParent() const
{
	return parent;
}

Matrix4x4 Transform::GetLocalMatrix() const
{
	return Matrix4x4::TRS(position, rotation, scale);
}

Matrix4x4 Transform::GetWorldMatrix() const
{
	const Matrix4x4 localMatrix = GetLocalMatrix();
	return parent ? (localMatrix * parent->GetWorldMatrix()) : localMatrix;
}
