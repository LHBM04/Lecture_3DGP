#pragma once

#include <span>
#include <vector>

#include "Component.h"
#include "Matrix4x4.h"
#include "Quaternion.h"
#include "Vector3D.h"

class Transform : public Component
{
public:
	Transform() noexcept = default;
	~Transform() noexcept override = default;

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
	void SetWorldMatrix(const Matrix4x4& matrix_);

	[[nodiscard]] Transform* GetParent() noexcept;
	[[nodiscard]] const Transform* GetParent() const noexcept;
	void SetParent(Transform* const parent_);

	[[nodiscard]] std::span<Transform* const> GetChildren() noexcept;
	[[nodiscard]] std::span<Transform* const> GetChildren() const noexcept;

private:
	void UpdateMatrices();

	Vector3D position{ Vector3D::GetZero() };
	Quaternion rotation{ Quaternion::GetIdentity() };
	Vector3D scale{ Vector3D::GetOne() };

	Transform* parent{ nullptr };
	std::vector<Transform*> children;

	Matrix4x4 cachedWorldMatrix{ Matrix4x4::GetIdentity() };
};
