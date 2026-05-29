#include "Precompiled.h"
#include "Transform.h"
#include <algorithm>
#include "Matrix4x4.h"

Transform::~Transform()
{
	if (parent)
	{
		auto& pc = parent->children;
		pc.erase(std::remove(pc.begin(), pc.end(), this), pc.end());
	}

	for (auto* child : children)
	{
		child->parent = nullptr;
		child->SetDirty();
	}
}

Matrix4x4 Transform::GetLocalMatrix() const
{
	Matrix4x4 result;
	result.SetTRS(position, rotation, scale);
	return result;
}

void Transform::SetWorldPosition(const Vector3D& position_)
{
	if (parent)
	{
		const Matrix4x4 parentWorldInverse{ parent->GetWorldMatrix().GetInverse() };
		SetLocalPosition(parentWorldInverse.MultiplyPoint(position_));
	}
	else
	{
		SetLocalPosition(position_);
	}
}

void Transform::SetWorldRotation(const Quaternion& rotation_)
{
	if (parent)
	{
		const Quaternion parentWorldInverse{ Quaternion::Inverse(parent->GetWorldRotation()) };
		SetLocalRotation(parentWorldInverse * rotation_);
	}
	else
	{
		SetLocalRotation(rotation_);
	}
}

void Transform::SetWorldScale(const Vector3D& scale_)
{
	if (parent)
	{
		const Vector3D pScale = parent->GetWorldScale();
		SetLocalScale(Vector3D(scale_.x / pScale.x, scale_.y / pScale.y, scale_.z / pScale.z));
	}
	else
	{
		SetLocalScale(scale_);
	}
}

const Matrix4x4& Transform::GetWorldMatrix() const
{
	if (isDirty)
	{
		if (parent)
		{
			cachedWorldMatrix = parent->GetWorldMatrix() * GetLocalMatrix();
		}
		else
		{
			cachedWorldMatrix = GetLocalMatrix();
		}
		isDirty = false;
	}
	return cachedWorldMatrix;
}

void Transform::SetParent(Transform* const parent_)
{
	if (parent == parent_)
	{
		return;
	}

	if (parent)
	{
		auto& pc = parent->children;
		pc.erase(std::remove(pc.begin(), pc.end(), this), pc.end());
	}

	parent = parent_;

	if (parent)
	{
		parent->children.push_back(this);
	}

	SetDirty();
}

void Transform::SetDirty()
{
	if (isDirty)
	{
		return;
	}

	isDirty = true;
	for (auto* child : children)
	{
		child->SetDirty();
	}
}
