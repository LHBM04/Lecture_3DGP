#pragma once

#include <vector>
#include "Component.h"
#include "Vector3D.h"
#include "Quaternion.h"
#include "Matrix4x4.h"

class Transform final : public Component<Transform>
{
public:
	Transform() = default;
	~Transform();

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

	[[nodiscard]] const Matrix4x4& GetWorldMatrix() const;

	[[nodiscard]] Transform* GetParent();
	[[nodiscard]] const Transform* GetParent() const;
	void SetParent(Transform* const parent_);

private:
	void SetDirty();

private:
	Vector3D position{ Vector3D::GetZero() };
	Quaternion rotation{ Quaternion::GetIdentity() };
	Vector3D scale{ Vector3D::GetOne() };

	Transform* parent{ nullptr };
	std::vector<Transform*> children;

	mutable Matrix4x4 cachedWorldMatrix{ Matrix4x4::GetIdentity() };
	mutable bool isDirty{ true };
};

inline const Vector3D& Transform::GetLocalPosition() const
{
	return position;
}

inline void Transform::SetLocalPosition(const Vector3D& position_)
{
	position = position_;
	SetDirty();
}

inline const Quaternion& Transform::GetLocalRotation() const
{
	return rotation;
}

inline void Transform::SetLocalRotation(const Quaternion& rotation_)
{
	rotation = rotation_;
	SetDirty();
}

inline const Vector3D& Transform::GetLocalScale() const
{
	return scale;
}

inline void Transform::SetLocalScale(const Vector3D& scale_)
{
	scale = scale_;
	SetDirty();
}

inline Vector3D Transform::GetWorldPosition() const
{
	return GetWorldMatrix().GetWorldPosition();
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

inline Transform* Transform::GetParent()
{
	return parent;
}

inline const Transform* Transform::GetParent() const
{
	return parent;
}
