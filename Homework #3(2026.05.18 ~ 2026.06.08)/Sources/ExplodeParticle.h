#pragma once

#include "Component.h"
#include "Vector3D.h"

class ExplodeParticle : public Component
{
public:
	ExplodeParticle() = default;
	~ExplodeParticle() override = default;

	void SetVelocity(const Vector3D& velocity_) noexcept;
	void SetLifeTime(float lifeTime_) noexcept;
	void SetGravity(float gravity_) noexcept;

protected:
	void OnUpdate() override;

private:
	Vector3D velocity{ Vector3D::GetZero() };
	float lifeTime{ 0.6f };
	float elapsedTime{ 0.0f };
	float gravity{ -18.0f };
};
