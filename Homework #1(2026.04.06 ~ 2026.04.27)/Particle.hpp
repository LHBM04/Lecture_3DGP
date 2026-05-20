#pragma once

#include "Component.hpp"
#include "Vector3D.hpp"

class Particle final : public Component
{
public:
	virtual ~Particle() override = default;

	[[nodiscard]] const Vector3D& GetVelocity() const;
	void SetVelocity(const Vector3D& velocity_);

	[[nodiscard]] float GetLifetime() const;
	void SetLifetime(float lifetime_);

protected:
	void OnAttach() override;
	void OnUpdate() override;

private:
	Vector3D velocity{ Vector3D::GetZero() };
	float lifetime{ 0.6f };
	float elapsedTime{ 0.0f };
	Vector3D initialScale{ Vector3D::GetOne() };
};
