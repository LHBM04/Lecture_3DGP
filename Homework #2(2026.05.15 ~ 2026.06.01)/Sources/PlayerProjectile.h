#pragma once

#include "Component.h"
#include "Vector3D.h"

class PlayerProjectile : public Component
{
public:
	PlayerProjectile() = default;
	~PlayerProjectile() override = default;

	void SetDirection(const Vector3D& direction_) noexcept;
	void SetSpeed(float speed_) noexcept;
	void SetLifeTime(float lifeTime_) noexcept;

protected:
	void OnAwake() override;
	void OnUpdate(float deltaTime_) override;
	void OnCollisionEnter(class Collider* other_) override;

private:
	Vector3D direction{ Vector3D::GetForward() };
	float speed{ 30.0f };
	float lifeTime{ 2.0f };
	float elapsedTime{ 0.0f };
};
