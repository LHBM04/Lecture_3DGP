#pragma once

#include "Component.h"
#include "Vector3D.h"

class EnemyController : public Component
{
public:
	EnemyController() = default;
	~EnemyController() override = default;

protected:
	void OnUpdate(float deltaTime_) override;
	void OnCollisionEnter(class Collider* other_) override;

private:
	float detectRange{ 12.0f };
	float moveSpeed{ 8.0f };
	float rotationSpeed{ 360.0f };
	float maxStepHeight{ 0.5f };

	float verticalVelocity{ 0.0f };
	float gravity{ -20.0f };
	bool isGrounded{ false };
	bool hasDetectedPlayer{ false };
};
