#pragma once

#include "Component.h"
#include <algorithm>

class PlayerController final : public Component
{
public:
	PlayerController() = default;
	virtual ~PlayerController() = default;

	[[nodiscard]] float GetMoveSpeed() const noexcept;
	void SetMoveSpeed(float unitsPerSecond_) noexcept;

	[[nodiscard]] float GetRotationSpeed() const noexcept;
	void SetRotationSpeed(float degreesPerSecond_) noexcept;

protected:
	virtual void OnUpdate() override;

private:
	float moveSpeed{ 10.0f };
	float rotationSpeed{ 90.0f };
};

inline float PlayerController::GetMoveSpeed() const noexcept
{
	return moveSpeed;
}

inline void PlayerController::SetMoveSpeed(float unitsPerSecond_) noexcept
{
	moveSpeed = std::max(0.0f, unitsPerSecond_);
}

inline float PlayerController::GetRotationSpeed() const noexcept
{
	return rotationSpeed;
}

inline void PlayerController::SetRotationSpeed(float degreesPerSecond_) noexcept
{
	rotationSpeed = degreesPerSecond_;
}
