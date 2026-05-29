#pragma once

#include <DirectXCollision.h>

#include "Component.h"

class CubeCollider;
class SphereCollider;
class Vector3D;

template <class TDerived>
class Collider : public Component<TDerived>
{
public:
	Collider() = default;
	~Collider() = default;

	[[nodiscard]] bool IsTrigger() const noexcept;
	void SetTrigger(bool isTrigger_) noexcept;

	bool IsIntersects(const Vector3D& rayOrigin_, const Vector3D& rayDir_, float& distance_) const noexcept;

private:
	bool isTrigger{ false };
};

template <class TDerived>
inline bool Collider<TDerived>::IsTrigger() const noexcept
{
	return isTrigger;
}

template <class TDerived>
inline void Collider<TDerived>::SetTrigger(bool isTrigger_) noexcept
{
	isTrigger = isTrigger_;
}

#include "Matrix4x4.h"
#include "Vector3D.h"

template <class TDerived>
inline bool Collider<TDerived>::IsIntersects(const Vector3D& rayOrigin_, const Vector3D& rayDir_, float& distance_) const noexcept
{
	// 자식이 구현한 GetVolume()을 호출하여 DirectX 라이브러리에 위임
	return static_cast<const TDerived*>(this)->GetVolume().Intersects(
		Vector3D::Load(rayOrigin_), 
		Vector3D::Load(rayDir_), 
		distance_);
}
