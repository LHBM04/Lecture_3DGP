#pragma once

#include "Component.hpp"
#include "Vector3D.hpp"

class CameraController final : public Component
{
public:
	virtual ~CameraController() override = default;

	[[nodiscard]] Transform* GetTarget() const;
	void SetTarget(Transform* target_);

	[[nodiscard]] const Vector3D& GetOffset() const;
	void SetOffset(const Vector3D& offset_);

protected:
	void OnLateUpdate() override;

private:
	Transform* target{ nullptr };

	Vector3D offset{ 0.0f, 3.0f, -7.0f };
	Vector3D followVelocity{ 0.0f, 0.0f, 0.0f };
};
