#pragma once

#include "Component.h"
#include "Vector3D.h"
#include <algorithm>

class Transform;

class CameraController final : public Component
{
public:
	CameraController() = default;
	~CameraController() override = default;

	CameraController(const CameraController&) = delete;
	CameraController& operator=(const CameraController&) = delete;

	CameraController(CameraController&&) = delete;
	CameraController& operator=(CameraController&&) = delete;

	[[nodiscard]] Transform* GetTarget() const noexcept;
	void SetTarget(Transform* target_) noexcept;

	[[nodiscard]] const Vector3D& GetOffset() const noexcept;
	void SetOffset(const Vector3D& offset_) noexcept;

	[[nodiscard]] float GetFollowSmoothTime() const noexcept;
	void SetFollowSmoothTime(float smoothTime_) noexcept;

protected:
	void OnUpdate() override;

private:
	Transform* target{ nullptr };
	Vector3D offset{ 0.0f, 3.0f, -7.0f };
	Vector3D followVelocity{ 0.0f, 0.0f, 0.0f };
	float followSmoothTime{ 0.125f };
};

inline Transform* CameraController::GetTarget() const noexcept
{
	return target;
}

inline void CameraController::SetTarget(Transform* target_) noexcept
{
	target = target_;
	followVelocity = Vector3D::GetZero();
}

inline const Vector3D& CameraController::GetOffset() const noexcept
{
	return offset;
}

inline void CameraController::SetOffset(const Vector3D& offset_) noexcept
{
	offset = offset_;
}

inline float CameraController::GetFollowSmoothTime() const noexcept
{
	return followSmoothTime;
}

inline void CameraController::SetFollowSmoothTime(float smoothTime_) noexcept
{
	followSmoothTime = std::max(0.01f, smoothTime_);
}
