#pragma once

#include "Transition.hpp"

namespace TUK::Framework
{
	class AssetSubsystem;
	class GPUCommandBuffer;
	class GPUDevice;
	class InputSubsystem;
	class World;
	class WorldSubsystem;
}

namespace TUK::Game
{
	class WorldTransition
	{
	public:
		WorldTransition() noexcept = default;

		[[nodiscard]] std::expected<void, std::string> Initialize(
			Framework::WorldSubsystem* worlds,
			Framework::InputSubsystem* input,
			const Framework::GPUDevice& device,
			Framework::AssetSubsystem* assets);
		void Shutdown() noexcept;

		[[nodiscard]] bool Request(Framework::World* world, Framework::f32 duration = 0.35f) noexcept;
		void Update(Framework::f32 deltaTime);
		[[nodiscard]] bool IsActive() const noexcept;
		[[nodiscard]] Transition& GetTransition() noexcept;
		[[nodiscard]] const Transition& GetTransition() const noexcept;

	private:
		enum class Phase : Framework::u8
		{
			IDLE,
			WAIT_FOR_EXIT,
			WAIT_FOR_WORLD,
			WAIT_FOR_ENTER
		};

		void BeginEnter() noexcept;
		void Finish() noexcept;

		Framework::WorldSubsystem* worlds = nullptr;
		Framework::InputSubsystem* input = nullptr;
		Transition transition;
		Framework::World* targetWorld = nullptr;
		Phase phase = Phase::IDLE;
		Framework::f32 fadeDuration = 0.35f;
		bool wasInputEnabled = true;
	};

	inline bool WorldTransition::IsActive() const noexcept
	{
		return phase != Phase::IDLE;
	}

	inline Transition& WorldTransition::GetTransition() noexcept
	{
		return transition;
	}

	inline const Transition& WorldTransition::GetTransition() const noexcept
	{
		return transition;
	}
}
