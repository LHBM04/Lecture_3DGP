#pragma once

#include <DirectXCollision.h>

#include "Collider.h"

class Vector3D;
class SphereCollider;
class CubeCollider;

class StairCollider final : public Collider
{
public:
	enum class SlopeAxis
	{
		PositiveZ,
		NegativeZ,
		PositiveX,
		NegativeX,
	};

	StairCollider() = default;
	~StairCollider() override = default;

	void SetCenter(const Vector3D& center_) noexcept;
	void SetSize(const Vector3D& size_) noexcept;
	void SetSlopeAxis(SlopeAxis axis_) noexcept;

	[[nodiscard]] bool IsIntersects(const Collider* other_) const override;
	[[nodiscard]] bool IsIntersects(const SphereCollider* other_) const override;
	[[nodiscard]] bool IsIntersects(const CubeCollider* other_) const override;

	[[nodiscard]] bool IsIntersects(const Vector3D& rayOrigin_, const Vector3D& rayDir_, float& distance_) const override;
	[[nodiscard]] bool IsIntersects(const DirectX::BoundingFrustum& frustum_) const override;
	[[nodiscard]] DirectX::BoundingBox GetBoundingVolume() const override;

	void UpdateVolume() override;

private:
	void OnUpdate(float deltaTime_) override;
	[[nodiscard]] bool IsPointInsideWedgeLocal(const Vector3D& pointLocal_) const;

private:
	Vector3D localCenter;
	Vector3D localSize{ 1.0f, 1.0f, 1.0f };
	SlopeAxis slopeAxis{ SlopeAxis::PositiveZ };

	DirectX::BoundingOrientedBox worldBox{ {0.0f, 0.0f, 0.0f}, {0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 0.0f, 1.0f} };
};
