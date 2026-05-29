#pragma once

#include "Component.h"
#include "Vector3D.h"
#include "Quaternion.h"
#include "Matrix4x4.h"

class Transform : public Component
{
public:
	Transform() = default;
	~Transform() override = default;

	[[nodiscard]] const Vector3D& GetLocalPosition() const;
	void SetLocalPosition(const Vector3D& position_);

	[[nodiscard]] const Quaternion& GetLocalRotation() const;
	void SetLocalRotation(const Quaternion& rotation_);

	[[nodiscard]] const Vector3D& GetLocalScale() const;
	void SetLocalScale(const Vector3D& scale_);

	[[nodiscard]] Matrix4x4 GetLocalMatrix() const;
	
	[[nodiscard]] Vector3D GetWorldPosition() const;
	void SetWorldPosition(const Vector3D& position_);

	[[nodiscard]] Quaternion GetWorldRotation() const;
	void SetWorldRotation(const Quaternion& rotation_);

	[[nodiscard]] Vector3D GetWorldScale() const;
	void SetWorldScale(const Vector3D& scale_);

	[[nodiscard]] Matrix4x4 GetWorldMatrix() const;

	[[nodiscard]] Transform* GetParent();
	[[nodiscard]] const Transform* GetParent() const;
	void SetParent(Transform* const parent_);


private:
	Vector3D position{ Vector3D::GetZero() };
	Quaternion rotation{ Quaternion::GetIdentity() };
	Vector3D scale{ Vector3D::GetOne() };

	Transform* parent{ nullptr };
};

inline const Vector3D& Transform::GetLocalPosition() const
{
	return position;
}

inline void Transform::SetLocalPosition(const Vector3D& position_)
{
	position = position_;
}

inline const Quaternion& Transform::GetLocalRotation() const
{
	return rotation;
}

inline void Transform::SetLocalRotation(const Quaternion& rotation_)
{
	rotation = rotation_;
}

inline const Vector3D& Transform::GetLocalScale() const
{
	return scale;
}

inline void Transform::SetLocalScale(const Vector3D& scale_)
{
	scale = scale_;
}

inline Matrix4x4 Transform::GetLocalMatrix() const
{
	Matrix4x4 result;
	result.SetTRS(position, rotation, scale);
	return result;
}

inline Vector3D Transform::GetWorldPosition() const
{
	return GetWorldMatrix().GetWorldPosition();
}

inline void Transform::SetWorldPosition(const Vector3D& position_)
{
	if (parent)
	{
		const Matrix4x4 parentWorldMatrix{ parent->GetWorldMatrix() };
		const Matrix4x4 parentWorldInverse{ parentWorldMatrix.GetInverse() };
		const Vector3D localPosition{ parentWorldInverse.MultiplyPoint(position_) };
		SetLocalPosition(localPosition);
	}
	else
	{
		SetLocalPosition(position_);
	}
}

inline Quaternion Transform::GetWorldRotation() const
{
	if (parent)
	{
		return parent->GetWorldRotation() * GetLocalRotation();
	}
	else
	{
		return GetLocalRotation();
	}
}

inline void Transform::SetWorldRotation(const Quaternion& rotation_)
{
	if (parent)
	{
		const Quaternion parentWorldRotation{ parent->GetWorldRotation() };
		const Quaternion parentWorldInverse{ Quaternion::Inverse(parentWorldRotation) };
		const Quaternion localRotation{ parentWorldInverse * rotation_ };
		SetLocalRotation(localRotation);
	}
	else
	{
		SetLocalRotation(rotation_);
	}
}

inline Vector3D Transform::GetWorldScale() const
{
	if (parent)
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

inline void Transform::SetWorldScale(const Vector3D& scale_)
{
	if (parent)
	{
		const Vector3D parentWorldScale{ parent->GetWorldScale() };
		const Vector3D localScale{ scale_.x / parentWorldScale.x, scale_.y / parentWorldScale.y, scale_.z / parentWorldScale.z };
		SetLocalScale(localScale);
	}
	else
	{
		SetLocalScale(scale_);
	}
}

inline Matrix4x4 Transform::GetWorldMatrix() const
{
	if (parent)
	{
		return parent->GetWorldMatrix() * GetLocalMatrix();
	}
	else
	{
		return GetLocalMatrix();
	}
}

inline Transform* Transform::GetParent()
{
	return parent;
}

inline const Transform* Transform::GetParent() const
{
	return parent;
}

inline void Transform::SetParent(Transform* const parent_)
{
	parent = parent_;
}
