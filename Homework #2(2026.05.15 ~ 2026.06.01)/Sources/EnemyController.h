#pragma once

#include "Component.h"
#include "Vector3D.h"

class EnemyController final : public Component
{
public:
	EnemyController() = default;
	~EnemyController() override = default;

protected:
	void OnAwake() override;
	void OnStart() override;

	void OnUpdate(float deltaTime_) override;

	void OnCollisionEnter(class Collider* other_) override;

private:
	[[nodiscard]] bool IsColliding(bool ignoreFloor_);
	void SpawnExplosionParticles(const Vector3D& origin_);

	float moveSpeed{ 5.0f };
	float detectionRange{ 15.0f };
	float rotationSpeed{ 120.0f };
	
	float verticalVelocity{ 0.0f };
	float gravity{ -20.0f };
	bool isGrounded{ false };

	bool hasDetectedPlayer{ false };
};
