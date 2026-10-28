#pragma once

#include "Component.hpp"

class Bullet final : public Component
{
public:
	virtual ~Bullet() override = default;

protected:
	void OnUpdate() override;

private:
	float speed{ 100.0f };

	float elapsedTime{ 0.0f };
	float lifetime{ 2.5f };
};
