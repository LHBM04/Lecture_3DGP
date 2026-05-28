#pragma once

#include <vector>

#include "Component.h"
#include "Matrix4x4.h"
#include "Quaternion.h"
#include "Vector3D.h"

class Transform : public Component
{
public:
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
	void SetParent(Transform* parent_);

	[[nodiscard]] Matrix4x4 GetLocalMatrix() const;
	[[nodiscard]] Matrix4x4 GetWorldMatrix() const;

private:
	Vector3D localPosition{ 0.0f, 0.0f, 0.0f };
	Quaternion localRotation{ 0.0f, 0.0f, 0.0f, 1.0f };
	Vector3D localScale{ 1.0f, 1.0f, 1.0f };

	Transform* parent{ nullptr };
	std::vector<Transform*> children;

	Matrix4x4 localMatrix{ Matrix4x4::GetIdentity() };
	Matrix4x4 worldMatrix{ Matrix4x4::GetIdentity() };
	bool localDirty{ true };
	bool worldDirty{ true };
};
