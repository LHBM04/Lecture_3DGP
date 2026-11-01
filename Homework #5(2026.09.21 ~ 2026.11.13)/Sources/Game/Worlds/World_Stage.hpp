#pragma once

#include "../../Framework/Entities/World.hpp"

namespace TUK::Game
{
	class World_Stage : public Framework::World
	{
	public:
		World_Stage() noexcept = default;
		~World_Stage() noexcept override = default;

	protected:
		void OnEnter() override;
		void OnUpdate() override;
	};
}
