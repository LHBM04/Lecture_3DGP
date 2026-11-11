#pragma once

#include "Framework/Objects.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	class WorldTransition;

	class World_Title : public World
	{
	public:
		explicit World_Title(World* stageWorld, WorldTransition* transition) noexcept;

		void OnEnter() override;
		void OnExit() override;

	private:
		World* stageWorld;
		WorldTransition* transition;
	};
}
