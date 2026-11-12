#pragma once

#include <memory>

#include "Framework/GameObjects.hpp"
#include "Framework/Resources.hpp"
#include "Framework/Platform.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	class Scene_Stage : public Framework::Scene
	{
		using Scene::Scene;

	protected:
		void OnEnter() override;
		void OnUpdate() override;

	private:
		AssetSubsystem* assets = nullptr;
		
		GameObject* tank = nullptr;
	};
}
