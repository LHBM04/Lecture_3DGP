#pragma once

#include "Component.h"
#include "Vector3D.h"

class Transform;

class CameraController : public Component
{
public:
	CameraController() = default;
	~CameraController() override = default;
	
	[[nodiscard]] Transform* GetTarget() const noexcept;
	void SetTarget(Transform* target_) noexcept;

	[[nodiscard]] const Vector3D& GetOffset() const noexcept;
	void SetOffset(const Vector3D& offset_) noexcept;

	[[nodiscard]] const Vector3D& GetFollowVelocity() const noexcept;
	void SetFollowVelocity(const Vector3D& velocity_) noexcept;

	[[nodiscard]] float GetSmoothTime() const noexcept;
	void SetSmoothTime(float smoothTime_) noexcept;

protected:
	void OnLateUpdate(float deltaTime_) override;

private:
	Transform* target{ nullptr };

	Vector3D offset{ 0.0f, 5.0f, -10.0f };
	Vector3D followVelocity{ 0.0f, 0.0f, 0.0f };

	float followSmoothTime{ 0.1f };
};
