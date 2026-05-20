#pragma once

#include "Component.hpp"
#include "Vector3D.hpp"

class RotatingObstacle final : public Component
{
public:
	virtual ~RotatingObstacle() override = default;

	[[nodiscard]] float GetRotationSpeed() const;
	void SetRotationSpeed(float rotationSpeed_);

	[[nodiscard]] float GetCollisionRadius() const;
	void SetCollisionRadius(float collisionRadius_);

	[[nodiscard]] float GetMovementSpeed() const;
	void SetMovementSpeed(float movementSpeed_);

	void SetDirectionChangeIntervalRange(float minInterval_, float maxInterval_);
	[[nodiscard]] int GetObstacleSize() const;
	void SetObstacleSize(int obstacleSize_);

protected:
	void OnAttach() override;
	void OnUpdate() override;

private:
	void ChooseNextMovementDirection();

	float rotationSpeed{ 90.0f };
	float collisionRadius{ 1.0f };
	float movementSpeed{ 3.0f };
	float minDirectionChangeInterval{ 1.5f };
	float maxDirectionChangeInterval{ 3.5f };
	float remainingDirectionChangeTime{ 0.0f };
	Vector3D movementDirection{ Vector3D::GetForward() };
	int obstacleSize{ 1 };
	std::mt19937 randomEngine{ std::random_device{}() };
};
