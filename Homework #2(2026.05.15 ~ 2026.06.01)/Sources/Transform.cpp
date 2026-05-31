#include "Precompiled.h"
#include "Transform.h"
#include "Matrix4x4.h"

void Transform::OnDestroy()
{
	if (parent != nullptr)
	{
		std::vector<Transform*>& pc{ parent->children };
		pc.erase(std::remove(pc.begin(), pc.end(), this), pc.end());
		parent = nullptr;
	}

	for (Transform* child : children)
	{
		child->parent = nullptr;
		child->UpdateMatrices();
	}
	children.clear();
}

const Vector3D& Transform::GetLocalPosition() const noexcept
{
	return position;
}

void Transform::SetLocalPosition(const Vector3D& position_)
{
	position = position_;
	UpdateMatrices();
}

const Quaternion& Transform::GetLocalRotation() const noexcept
{
	return rotation;
}

void Transform::SetLocalRotation(const Quaternion& rotation_)
{
	rotation = rotation_;
	UpdateMatrices();
}

const Vector3D& Transform::GetLocalScale() const noexcept
{
	return scale;
}

void Transform::SetLocalScale(const Vector3D& scale_)
{
	scale = scale_;
	UpdateMatrices();
}

Matrix4x4 Transform::GetLocalMatrix() const noexcept
{
	Matrix4x4 result;
	result.SetTRS(position, rotation, scale);
	return result;
}

Vector3D Transform::GetWorldPosition() const noexcept
{
	return GetWorldMatrix().GetWorldPosition();
}

void Transform::SetWorldPosition(const Vector3D& position_)
{
	if (parent != nullptr)
	{
		const Matrix4x4 parentWorldInverse{ parent->GetWorldMatrix().GetInverse() };
		SetLocalPosition(parentWorldInverse.MultiplyPoint(position_));
	}
	else
	{
		SetLocalPosition(position_);
	}
}

Quaternion Transform::GetWorldRotation() const noexcept
{
	if (parent != nullptr)
	{
		return parent->GetWorldRotation() * GetLocalRotation();
	}
	else
	{
		return GetLocalRotation();
	}
}

void Transform::SetWorldRotation(const Quaternion& rotation_)
{
	if (parent != nullptr)
	{
		const Quaternion parentWorldInverse{ Quaternion::Inverse(parent->GetWorldRotation()) };
		SetLocalRotation(parentWorldInverse * rotation_);
	}
	else
	{
		SetLocalRotation(rotation_);
	}
}

Vector3D Transform::GetWorldScale() const noexcept
{
	if (parent != nullptr)
	{
		const Vector3D parentWorldScale{ parent->GetWorldScale() };
		return Vector3D(
			scale.x * parentWorldScale.x,
			scale.y * parentWorldScale.y,
			scale.z * parentWorldScale.z);
	}
	else
	{
		return scale;
	}
}

void Transform::SetWorldScale(const Vector3D& scale_)
{
	if (parent != nullptr)
	{
		const Vector3D pScale{ parent->GetWorldScale() };
		SetLocalScale(Vector3D(scale_.x / pScale.x, scale_.y / pScale.y, scale_.z / pScale.z));
	}
	else
	{
		SetLocalScale(scale_);
	}
}

const Matrix4x4& Transform::GetWorldMatrix() const noexcept
{
	return cachedWorldMatrix;
}

void Transform::SetWorldMatrix(const Matrix4x4& matrix_)
{
	if (parent != nullptr)
	{
		Matrix4x4 parentInverse{ parent->GetWorldMatrix().GetInverse() };
		Matrix4x4 localMatrix{ matrix_ * parentInverse };
		
		scale = localMatrix.GetScale();
		rotation = localMatrix.GetRotation();
		position = localMatrix.GetWorldPosition();
	}
	else
	{
		scale = matrix_.GetScale();
		rotation = matrix_.GetRotation();
		position = matrix_.GetWorldPosition();
	}

	UpdateMatrices();
}

Transform* Transform::GetParent() noexcept
{
	return parent;
}

const Transform* Transform::GetParent() const noexcept
{
	return parent;
}

std::span<Transform* const> Transform::GetChildren() noexcept
{
	return children;
}

std::span<Transform* const> Transform::GetChildren() const noexcept
{
	return children;
}

void Transform::SetParent(Transform* const parent_)
{
	if (parent == parent_)
	{
		return;
	}

	if (parent != nullptr)
	{
		std::vector<Transform*>& pc{ parent->children };
		pc.erase(std::remove(pc.begin(), pc.end(), this), pc.end());
	}

	parent = parent_;

	if (parent != nullptr)
	{
		parent->children.push_back(this);
	}

	UpdateMatrices();
}

void Transform::UpdateMatrices()
{
	if (parent != nullptr)
	{
		cachedWorldMatrix = GetLocalMatrix() * parent->GetWorldMatrix();
	}
	else
	{
		cachedWorldMatrix = GetLocalMatrix();
	}

	for (Transform* child : children)
	{
		child->UpdateMatrices();
	}
}
