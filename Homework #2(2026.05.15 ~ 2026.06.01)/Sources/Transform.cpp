#include "Precompiled.h"
#include "Transform.h"

#include <algorithm>

void Transform::SetLocalPosition(const Vector3D& position_)
{
	position = position_;
	SetDirty();
}

void Transform::SetLocalRotation(const Quaternion& rotation_)
{
	rotation = rotation_;
	SetDirty();
}

void Transform::SetLocalScale(const Vector3D& scale_)
{
	scale = scale_;
	SetDirty();
}

const Vector3D& Transform::GetLocalPosition() const
{
	return position;
}

const Quaternion& Transform::GetLocalRotation() const
{
	return rotation;
}

const Vector3D& Transform::GetLocalScale() const
{
	return scale;
}

Transform* Transform::GetParent()
{
	return parent;
}

const Transform* Transform::GetParent() const
{
	return parent;
}

Vector3D Transform::GetWorldPosition() const
{
	return GetWorldMatrix().GetWorldPosition();
}

void Transform::SetWorldPosition(const Vector3D& position_)
{
	if (nullptr == parent)
	{
		SetLocalPosition(position_);
		return;
	}

	SetLocalPosition(parent->GetWorldMatrix().GetInverse().MultiplyPoint(position_));
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
		SetLocalRotation(rotation_);
		return;
	}

	SetLocalRotation(Quaternion::Inverse(parent->GetWorldRotation()) * rotation_);
}

Vector3D Transform::GetWorldScale() const
{
	return GetWorldMatrix().GetScale();
}

void Transform::SetWorldScale(const Vector3D& scale_)
{
	if (nullptr == parent)
	{
		SetLocalScale(scale_);
		return;
	}

	const Vector3D parentScale{ parent->GetWorldScale() };
	SetLocalScale(Vector3D(
		parentScale.x != 0.0f ? scale_.x / parentScale.x : scale_.x,
		parentScale.y != 0.0f ? scale_.y / parentScale.y : scale_.y,
		parentScale.z != 0.0f ? scale_.z / parentScale.z : scale_.z));
}

void Transform::SetParent(Transform* parent_)
{
	if (parent == parent_)
	{
		return;
	}

	if (nullptr != parent)
	{
		std::erase(parent->children, this);
	}

	parent = parent_;

	if (nullptr != parent)
	{
		parent->children.push_back(this);
	}

	SetDirty();
}

Matrix4x4 Transform::GetLocalMatrix() const
{
	if (localDirty)
	{
		localMatrix = Matrix4x4::TRS(position, rotation, scale);
		localDirty = false;
	}

	return localMatrix;
}

Matrix4x4 Transform::GetWorldMatrix() const
{
	if (worldDirty)
	{
		if (nullptr == parent)
		{
			worldMatrix = GetLocalMatrix();
		}
		else
		{
			worldMatrix = GetLocalMatrix() * parent->GetWorldMatrix();
		}
		worldDirty = false;
	}

	return worldMatrix;
}

void Transform::SetDirty()
{
	localDirty = true;
	worldDirty = true;

	for (Transform* child : children)
	{
		if (nullptr != child && !child->worldDirty)
		{
			child->SetDirty();
		}
	}
}
