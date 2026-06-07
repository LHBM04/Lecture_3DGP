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

	[[nodiscard]] bool IsFirstPerson() const noexcept;
	void SetFirstPerson(bool isFirstPerson_) noexcept;

	[[nodiscard]] const Vector3D& GetFirstPersonOffset() const noexcept;
	void SetFirstPersonOffset(const Vector3D& offset_) noexcept;

protected:
	void OnLateUpdate() override;

private:
	Transform* target{ nullptr };

	Vector3D offset{ 0.8f, 5.0f, -10.0f };
	Vector3D firstPersonOffset{ 2.17f, 4.5f, 14.0f };
	Vector3D followVelocity{ 0.0f, 0.0f, 0.0f };

	float followSmoothTime{ 0.1f };
	bool isFirstPerson{ false };
};
