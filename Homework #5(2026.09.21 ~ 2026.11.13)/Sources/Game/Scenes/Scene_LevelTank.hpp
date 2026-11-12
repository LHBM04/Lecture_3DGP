#pragma once

#include "Framework/Objects.hpp"
#include "Framework/Platform.hpp"
#include "Framework/UI.hpp"

namespace TUK::Game
{
	using namespace Framework;

	class Scene_LevelTank : public Scene
	{
	public:
		using Scene::Scene;

		[[nodiscard]] usize GetEnemyCount() const noexcept;

	protected:
		void OnEnter() override;
		void OnLateUpdate() override;
		void OnExit() override;

	private:
		usize enemyCount = 0;
		usize totalEnemyCount = 10;
		i32 maxPlayerHealth = 10;

		bool isGameOver = false;

		TextView* hudTextView = nullptr;
		InputAction::ActionCallback::ListenerId cancelListenerId = -1;
	};

	inline usize Scene_LevelTank::GetEnemyCount() const noexcept
	{
		return enemyCount;
	}
}
