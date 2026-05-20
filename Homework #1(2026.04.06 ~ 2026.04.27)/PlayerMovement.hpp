#pragma once

#include "Component.hpp"

class PlayerMovement : public Component
{
public:
	virtual ~PlayerMovement() override = default;

	[[nodiscard]] float GetMoveSpeed() const;
	void SetMoveSpeed(float moveSpeed_);

protected:
	virtual void OnAttach() override;
	virtual void OnUpdate() override;

private:
	float moveSpeed{ 10.0f };
	float rotationSpeed{ 120.0f };
	float fireCooldown{ 0.12f };
	float fireTimer{ 0.0f };
	float bulletSpawnForwardOffset{ 2.2f };
	float bulletSpawnUpOffset{ 0.2f };
};
