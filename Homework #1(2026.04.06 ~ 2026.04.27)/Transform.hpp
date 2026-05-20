#pragma once

#include "Component.hpp"
#include "Matrix4x4.hpp"
#include "Vector3D.hpp"
#include "Quaternion.hpp"

class Transform final : public Component
{
public:
	virtual ~Transform() = default;

	[[nodiscard]] const Vector3D& GetLocalPosition() const;
	void SetLocalPosition(const Vector3D& position_);

	[[nodiscard]] const Quaternion& GetLocalRotation() const;
	void SetLocalRotation(const Quaternion& rotation_);

	[[nodiscard]] const Vector3D& GetLocalScale() const;
	void SetLocalScale(const Vector3D& scale_);

	[[nodiscard]] Vector3D GetWorldPosition() const;
	void SetWorldPosition(const Vector3D& position_);

	[[nodiscard]] Quaternion GetWorldRotation() const;
	void SetWorldRotation(const Quaternion& rotation_);

	[[nodiscard]] Vector3D GetWorldScale() const;
	void SetWorldScale(const Vector3D& scale_);

	[[nodiscard]] Transform* GetParent();
	[[nodiscard]] const Transform* GetParent() const;

	[[nodiscard]] Matrix4x4 GetLocalMatrix() const;
	[[nodiscard]] Matrix4x4 GetWorldMatrix() const;

private:
	Vector3D position{ Vector3D::GetZero() };
	Quaternion rotation{ Quaternion::GetIdentity() };
	Vector3D scale{ Vector3D::GetOne() };

	Transform* parent{ nullptr };
};
