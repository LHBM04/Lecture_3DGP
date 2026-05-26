#pragma once

#include "Collider.h"
#include "Vector3D.h"

class CubeCollider final : public Collider
{
public:
	~CubeCollider() override = default;

	[[nodiscard]] const Vector3D& GetCenter() const noexcept;
	void SetCenter(const Vector3D& center_) noexcept;

	[[nodiscard]] const Vector3D& GetSize() const noexcept;
	void SetSize(const Vector3D& size_) noexcept;

	[[nodiscard]] bool Intersects(const DirectX::BoundingFrustum& frustum_) const override;

private:
	[[nodiscard]] DirectX::BoundingOrientedBox BuildWorldOrientedBox() const;

	Vector3D center{ Vector3D::GetZero() };
	Vector3D size{ Vector3D::GetOne() };
};
