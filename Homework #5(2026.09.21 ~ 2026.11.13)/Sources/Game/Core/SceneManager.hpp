#pragma once

#include "Framework/Objects.hpp"

#include "../UI/SceneTransition.hpp"

namespace TUK::Game
{
	using namespace Framework;

	namespace SceneManager
	{
		void Initialize(SceneSubsystem* subsystem) noexcept;
		void Release() noexcept;

		Scene* AddScene(std::string_view name, std::unique_ptr<Scene> scene);
		[[nodiscard]] Scene* FindSceneByName(std::string_view name) noexcept;
		[[nodiscard]] Scene* GetCurrentScene() noexcept;

		bool LoadScene(Scene* targetScene, f32 duration = 0.35f);
		bool LoadScene(std::string_view name, f32 duration = 0.35f);

		[[nodiscard]] bool IsTransitioning() noexcept;
	}
}
