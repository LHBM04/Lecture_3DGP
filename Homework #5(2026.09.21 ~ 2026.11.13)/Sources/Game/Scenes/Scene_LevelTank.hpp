#pragma once

#include "Framework/Objects.hpp"
#include "Framework/Platform.hpp"
#include "Framework/Resources.hpp"

namespace TUK::Game
{
	using namespace Framework;

	class Scene_LevelTank : public Scene
	{
		using Scene::Scene;

	protected:
		void OnEnter() override;
		void OnExit() override;

	private:
		InputAction::ActionCallback::ListenerId cancelListenerID = - 1;
	};
}
