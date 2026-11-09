#pragma once

#include "Framework/Entities.hpp"
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
		InputSubsystem* input = nullptr;

		Transform* cameraTransform = nullptr;
		f32 cameraYaw = 0.0f;
		f32 cameraPitch = 0.0f;

		Entity* tank = nullptr;
	};
}
