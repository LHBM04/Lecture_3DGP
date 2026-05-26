#pragma once

#include "Component.h"
#include "Updatable.h"
#include "Vector3D.h"

class Transform;
struct TimeContext;

class CameraController final : public Component, public Updatable
{
public:
	CameraController() = default;
	~CameraController() override = default;

	void SetTarget(Transform* target_) { target = target_; }
	void SetOffset(const Vector3D& offset_) { offset = offset_; }
	void SetSmoothTime(float smoothTime_) { followSmoothTime = smoothTime_; }

protected:
	void OnUpdate(const TimeContext& context_) override;

private:
	Transform* target{ nullptr };
	Vector3D offset{ 0.0f, 5.0f, -10.0f };
	Vector3D followVelocity{ 0.0f, 0.0f, 0.0f };
	float followSmoothTime{ 0.1f };
};
