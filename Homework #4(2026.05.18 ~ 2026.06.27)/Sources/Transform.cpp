#include "Precompiled.h"
#include "Transform.h"

void Transform::OnDestroy()
{
	SetParent(nullptr);

	for (Transform* child : children)
	{
		if (child != nullptr)
		{
			child->parent = nullptr;
			child->UpdateMatrices();
		}
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
	return Matrix4x4::TRS(position, rotation, scale);
}

Vector3D Transform::GetWorldPosition() const noexcept
{
	return cachedWorldMatrix.GetWorldPosition();
}

void Transform::SetWorldPosition(const Vector3D& position_)
{
	if (parent != nullptr)
	{
		position = parent->GetWorldMatrix().GetInverse().MultiplyPoint(position_);
	}
	else
	{
		position = position_;
	}

	UpdateMatrices();
}

Quaternion Transform::GetWorldRotation() const noexcept
{
	return cachedWorldMatrix.GetRotation();
}

void Transform::SetWorldRotation(const Quaternion& rotation_)
{
	if (parent != nullptr)
	{
		rotation = Quaternion::Inverse(parent->GetWorldRotation()) * rotation_;
	}
	else
	{
		rotation = rotation_;
	}

	UpdateMatrices();
}

Vector3D Transform::GetWorldScale() const noexcept
{
	return cachedWorldMatrix.GetLossyScale();
}

void Transform::SetWorldScale(const Vector3D& scale_)
{
	if (parent != nullptr)
	{
		const Vector3D parentScale{ parent->GetWorldScale() };
		scale = Vector3D(
			parentScale.x == 0.0f ? scale_.x : scale_.x / parentScale.x,
			parentScale.y == 0.0f ? scale_.y : scale_.y / parentScale.y,
			parentScale.z == 0.0f ? scale_.z : scale_.z / parentScale.z);
	}
	else
	{
		scale = scale_;
	}

	UpdateMatrices();
}

const Matrix4x4& Transform::GetWorldMatrix() const noexcept
{
	return cachedWorldMatrix;
}

void Transform::SetWorldMatrix(const Matrix4x4& matrix_)
{
	if (parent != nullptr)
	{
		cachedWorldMatrix = matrix_;
		const Matrix4x4 localMatrix{ matrix_ * parent->GetWorldMatrix().GetInverse() };
		position = localMatrix.GetWorldPosition();
		rotation = localMatrix.GetRotation();
		scale = localMatrix.GetLossyScale();
	}
	else
	{
		position = matrix_.GetWorldPosition();
		rotation = matrix_.GetRotation();
		scale = matrix_.GetLossyScale();
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

void Transform::SetParent(Transform* const parent_)
{
	if (parent == parent_)
	{
		return;
	}

	if (parent != nullptr)
	{
		std::erase(parent->children, this);
	}

	parent = parent_;
	if (parent != nullptr && !std::ranges::contains(parent->children, this))
	{
		parent->children.push_back(this);
	}

	UpdateMatrices();
}

std::span<Transform* const> Transform::GetChildren() noexcept
{
	return { children.data(), children.size() };
}

std::span<Transform* const> Transform::GetChildren() const noexcept
{
	return { children.data(), children.size() };
}

void Transform::UpdateMatrices()
{
	cachedWorldMatrix = GetLocalMatrix();
	if (parent != nullptr)
	{
		cachedWorldMatrix *= parent->GetWorldMatrix();
	}

	for (Transform* child : children)
	{
		if (child != nullptr)
		{
			child->UpdateMatrices();
		}
	}
}
