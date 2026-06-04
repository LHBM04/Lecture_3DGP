#pragma once

#include "Singleton.h"

class PhysicsSystem final : public Singleton<PhysicsSystem>
{
public:
	PhysicsSystem() = default;
	~PhysicsSystem() override = default;
};
