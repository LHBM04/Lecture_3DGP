#pragma once

#include <string>

#include "Component.h"
#include "Vector3D.h"

class Transform;
class GameObject;

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

	void SetFirstPersonOffset(const Vector3D& offset_) noexcept;
	void SetThirdPersonOffset(const Vector3D& offset_) noexcept;
	void SetCrosshairObject(GameObject* crosshair_) noexcept;

protected:
	void OnLateUpdate(float deltaTime_) override;

private:
	[[nodiscard]] Transform* ResolveTarget();

	GameObject* crosshairObject{ nullptr };
	std::wstring targetTag;
	std::wstring targetName;
	bool useTargetTag{ false };

	Vector3D offset{ 0.0f, 5.0f, -10.0f };
	Vector3D firstPersonOffset{ 0.0f, 1.6f, 0.0f };
	Vector3D thirdPersonOffset{ 0.0f, 2.0f, -3.0f };
	Vector3D followVelocity{ 0.0f, 0.0f, 0.0f };
	bool isFirstPerson{ false };

	float followSmoothTime{ 0.1f };
};
