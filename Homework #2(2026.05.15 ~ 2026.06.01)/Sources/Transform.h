#pragma once

#include <vector>
#include "Component.h"
#include "Vector3D.h"
#include "Quaternion.h"
#include "Matrix4x4.h"

class Transform final : public Component
{
public:
	Transform() = default;
	~Transform() override = default;

	void OnDestroy() override;

	[[nodiscard]] const Vector3D& GetLocalPosition() const noexcept;
	void SetLocalPosition(const Vector3D& position_);

	[[nodiscard]] const Quaternion& GetLocalRotation() const noexcept;
	void SetLocalRotation(const Quaternion& rotation_);

	[[nodiscard]] const Vector3D& GetLocalScale() const noexcept;
	void SetLocalScale(const Vector3D& scale_);

	[[nodiscard]] Matrix4x4 GetLocalMatrix() const noexcept;
	
	[[nodiscard]] Vector3D GetWorldPosition() const noexcept;
	void SetWorldPosition(const Vector3D& position_);

	[[nodiscard]] Quaternion GetWorldRotation() const noexcept;
	void SetWorldRotation(const Quaternion& rotation_);

	[[nodiscard]] Vector3D GetWorldScale() const noexcept;
	void SetWorldScale(const Vector3D& scale_);

	[[nodiscard]] const Matrix4x4& GetWorldMatrix() const noexcept;

	[[nodiscard]] Transform* GetParent() noexcept;
	[[nodiscard]] const Transform* GetParent() const noexcept;
	void SetParent(Transform* const parent_);

private:
	void UpdateMatrices();

private:
	Vector3D position{ Vector3D::GetZero() };
	Quaternion rotation{ Quaternion::GetIdentity() };
	Vector3D scale{ Vector3D::GetOne() };

	Transform* parent{ nullptr };
	std::vector<Transform*> children;

	Matrix4x4 cachedWorldMatrix{ Matrix4x4::GetIdentity() };
};

inline const Vector3D& Transform::GetLocalPosition() const noexcept
{
	return position;
}

inline void Transform::SetLocalPosition(const Vector3D& position_)
{
	position = position_;
	UpdateMatrices();
}

inline const Quaternion& Transform::GetLocalRotation() const noexcept
{
	return rotation;
}

inline void Transform::SetLocalRotation(const Quaternion& rotation_)
{
	rotation = rotation_;
	UpdateMatrices();
}

inline const Vector3D& Transform::GetLocalScale() const noexcept
{
	return scale;
}

inline void Transform::SetLocalScale(const Vector3D& scale_)
{
	scale = scale_;
	UpdateMatrices();
}

inline Vector3D Transform::GetWorldPosition() const noexcept
{
	return GetWorldMatrix().GetWorldPosition();
}

inline Quaternion Transform::GetWorldRotation() const noexcept
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

inline Vector3D Transform::GetWorldScale() const noexcept
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

inline Transform* Transform::GetParent() noexcept
{
	return parent;
}

inline const Transform* Transform::GetParent() const noexcept
{
	return parent;
}

