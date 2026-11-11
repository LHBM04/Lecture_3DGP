#pragma once

#include "Framework/Entities.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	class World_Title : public World
	{
	public:
		explicit World_Title(World* stageWorld) noexcept;

		void OnEnter() override;
		void OnExit() override;

	private:
		World* stageWorld;
	};
}
