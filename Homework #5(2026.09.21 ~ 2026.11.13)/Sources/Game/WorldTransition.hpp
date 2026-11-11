#pragma once

#include <expected>
#include <optional>
#include <string>

#include "Framework/Core/Common.hpp"

#include "Framework/Graphics/GPUGraphicsPipelineState.hpp"

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
		[[nodiscard]] std::expected<void, std::string> Render(Framework::GPUCommandBuffer& commandBuffer);

		[[nodiscard]] bool IsActive() const noexcept;

	private:
		enum class Phase : Framework::u8
		{
			IDLE,
			FADE_OUT,
			BLACK_FRAME,
			WAIT_FOR_WORLD,
			FADE_IN
		};

		void Finish() noexcept;

		static constexpr DXGI_FORMAT TARGET_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

		Framework::WorldSubsystem* worlds = nullptr;
		Framework::InputSubsystem* input = nullptr;
		std::optional<Framework::GPUGraphicsPipelineState> pipeline;
		Framework::World* targetWorld = nullptr;
		Phase phase = Phase::IDLE;
		Framework::f32 opacity = 0.0f;
		Framework::f32 fadeDuration = 0.35f;
		bool hasRenderedBlackFrame = false;
		bool wasInputEnabled = true;
	};

	inline bool WorldTransition::IsActive() const noexcept
	{
		return phase != Phase::IDLE;
	}
}
