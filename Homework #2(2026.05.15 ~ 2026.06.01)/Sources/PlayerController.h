#pragma once

#include "Component.h"
#include "Vector3D.h"

class PlayerController : public Component
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
};
