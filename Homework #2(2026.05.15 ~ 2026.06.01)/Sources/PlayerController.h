#pragma once

#include "Component.h"
#include "InputHandler.h"
#include "Updatable.h"
#include <algorithm>

struct TimeContext;

class PlayerController final : public Component, public Updatable, public InputHandler
{
public:
	PlayerController() = default;
	virtual ~PlayerController() = default;

	[[nodiscard]] float GetMoveSpeed() const noexcept;
	void SetMoveSpeed(float unitsPerSecond_) noexcept;

	[[nodiscard]] float GetRotationSpeed() const noexcept;
	void SetRotationSpeed(float degreesPerSecond_) noexcept;

protected:
	virtual void OnUpdate(const TimeContext& context_) override;
	void OnInputStarted(const InputContext& context_) override;
	void OnInputPerformed(const InputContext& context_) override;
	void OnInputCanceled(const InputContext& context_) override;

private:
	float moveSpeed{ 10.0f };
	float rotationSpeed{ 90.0f };
	float yawInput{ 0.0f };
	float moveInput{ 0.0f };
	bool jumpRequested{ false };
	bool dodgeRequested{ false };
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
