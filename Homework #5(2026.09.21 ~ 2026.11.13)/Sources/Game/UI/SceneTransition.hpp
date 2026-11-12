#pragma once

#include "Transition.hpp"

namespace TUK::Framework
{
	class AssetSubsystem;
	class GPUCommandBuffer;
	class GPUDevice;
	class InputSubsystem;
	class Scene;
	class SceneSubsystem;
}

namespace TUK::Game
{
	class SceneTransition
	{
	public:
		SceneTransition() noexcept = default;

		[[nodiscard]] std::expected<void, std::string> Initialize(
			Framework::SceneSubsystem* scenes,
			Framework::InputSubsystem* input,
			const Framework::GPUDevice& device,
			Framework::AssetSubsystem* assets);
		void Shutdown() noexcept;

		[[nodiscard]] bool Request(Framework::Scene* scene, Framework::f32 duration = 0.35f) noexcept;
		void Update(Framework::f32 deltaTime);
		[[nodiscard]] bool IsActive() const noexcept;
		[[nodiscard]] Transition& GetTransition() noexcept;
		[[nodiscard]] const Transition& GetTransition() const noexcept;

	private:
		enum class Phase : Framework::u8
		{
			IDLE,
			WAIT_FOR_EXIT,
			WAIT_FOR_SCENE,
			WAIT_FOR_ENTER
		};

		void BeginEnter() noexcept;
		void Finish() noexcept;

		Framework::SceneSubsystem* scenes = nullptr;
		Framework::InputSubsystem* input = nullptr;
		Transition transition;
		Framework::Scene* targetScene = nullptr;
		Phase phase = Phase::IDLE;
		Framework::f32 fadeDuration = 0.35f;
		bool wasInputEnabled = true;
	};

	inline bool SceneTransition::IsActive() const noexcept
	{
		return phase != Phase::IDLE;
	}

	inline Transition& SceneTransition::GetTransition() noexcept
	{
		return transition;
	}

	inline const Transition& SceneTransition::GetTransition() const noexcept
	{
		return transition;
	}
}
