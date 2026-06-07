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
	void OnStart() override;
	void OnUpdate() override;
	void OnCollisionEnter(class Collider* other_) override;

private:
	[[nodiscard]] bool IsColliding(bool ignoreFloor_);

	Transform* headTransform{ nullptr };
	float moveSpeed{ 10.0f };
	float altitudeSpeed{ 10.0f };
	float rotationSpeed{ 180.0f };

	float fireTimer{ 0.0f };
	float fireCooldown{ 0.2f };
	float projectileSpeed{ 80.0f };
	float projectileLifetime{ 2.0f };
	bool gameOverTriggered{ false };
};
