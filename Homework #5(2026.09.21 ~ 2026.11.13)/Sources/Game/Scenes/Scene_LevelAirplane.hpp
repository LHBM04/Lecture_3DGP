#pragma once

#include "Framework/GameObjects.hpp"
#include "Framework/Platform.hpp"

namespace TUK::Game
{
	using namespace Framework;

	class Scene_LevelAirplane : public Scene
	{
		using Scene::Scene;

	protected:
		void OnEnter() override;
		void OnExit() override;

	private:
		InputAction::ActionCallback::ListenerId returnListenerID = -1;
	};
}
