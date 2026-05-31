#pragma once

#include "Component.h"
#include "Vector3D.h"

class CubeCollider;
class Transform;

class PlayerController final : public Component
{
public:
	PlayerController() = default;
	~PlayerController() override = default;

protected:
	void OnAwake() override;
	void OnStart() override;

	void OnUpdate(float deltaTime_) override;

private:
	float moveSpeed{ 10.0f };
	float rotationSpeed{ 180.0f };

	float verticalVelocity{ 0.0f };
	float gravity{ -20.0f };
	float jumpSpeed{ 8.0f };
	bool isGrounded{ false };

	float fireTimer{ 0.0f };
	float fireCooldown{ 0.2f };
	float projectileSpeed{ 40.0f };
	float projectileLifetime{ 2.0f };
};
