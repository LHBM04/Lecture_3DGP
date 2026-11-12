#pragma once

#include "Framework/Core.hpp"
#include "Framework/Objects.hpp"

namespace TUK::Game
{
	using namespace Framework;

	class Scene_Title : public Scene
	{
		using Scene::Scene;

	public:
		void OnEnter() override;
	};
}
