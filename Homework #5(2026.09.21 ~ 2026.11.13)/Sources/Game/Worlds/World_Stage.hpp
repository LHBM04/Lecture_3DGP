#pragma once

#include <memory>

#include "Framework/Objects.hpp"
#include "Framework/Resources.hpp"
#include "Framework/Platform.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	class World_Stage : public Framework::World
	{
		using World::World;

	protected:
		void OnEnter() override;
		void OnUpdate() override;

	private:
		AssetSubsystem* assets = nullptr;
		
		GameObject* tank = nullptr;
	};
}
