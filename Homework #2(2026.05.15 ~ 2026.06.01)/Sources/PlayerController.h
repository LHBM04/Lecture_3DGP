#pragma once

#include "Component.h"

class PlayerController final : public Component
{
public:
	PlayerController() = default;
	virtual ~PlayerController() = default;

	PlayerController(const PlayerController&) = delete;
	PlayerController& operator=(const PlayerController&) = delete;

	PlayerController(PlayerController&&) = delete;
	PlayerController& operator=(PlayerController&&) = delete;

	[[nodiscard]] float GetRotationSpeed() const noexcept;
	void SetRotationSpeed(float degreesPerFrame_) noexcept;

protected:
	virtual void OnUpdate() override;

private:
	float rotationSpeed{ 1.5f };
};

inline float PlayerController::GetRotationSpeed() const noexcept
{
	return rotationSpeed;
}

inline void PlayerController::SetRotationSpeed(float degreesPerFrame_) noexcept
{
	rotationSpeed = degreesPerFrame_;
}
