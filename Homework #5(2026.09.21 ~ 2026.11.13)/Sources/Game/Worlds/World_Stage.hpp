#pragma once

#include "Framework/Entities.hpp"
#include "Framework/Resources.hpp"
#include "Framework/Platform.hpp"

namespace TUK::Game
{
	class World_Stage : public Framework::World
	{
		using World::World;

	protected:
		void OnEnter() override;
		void OnUpdate() override;

	private:
		Framework::AssetSubsystem* assets = nullptr;
		Framework::InputSubsystem* input = nullptr;
	};
}
