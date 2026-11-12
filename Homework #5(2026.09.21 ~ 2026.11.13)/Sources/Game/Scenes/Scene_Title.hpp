#pragma once

#include "Framework/GameObjects.hpp"

using namespace TUK::Framework;

namespace TUK::Game
{
	class SceneTransition;

	class Scene_Title : public Scene
	{
	public:
		explicit Scene_Title(Scene* stageScene, SceneTransition* transition) noexcept;

		void OnEnter() override;
		void OnExit() override;

	private:
		Scene* stageScene;
		SceneTransition* transition;
	};
}
