#pragma once

#include "Framework/Entities.hpp"

namespace TUK::Game
{
	class World_Stage : public Framework::World
	{
		using World::World;

	public:
		void OnEnter() override;
		void OnUpdate() override;
	};
}
