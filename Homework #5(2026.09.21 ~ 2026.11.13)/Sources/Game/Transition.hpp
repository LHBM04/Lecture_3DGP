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
}

namespace TUK::Game
{
	enum class TransitionState : Framework::u8
	{
		IDLE,
		ENTER,
		ENTERCOMPLETE,
		EXIT,
		EXITCOMPLETE
	};

	class Transition
	{
	public:
		Transition() noexcept = default;

		[[nodiscard]] std::expected<void, std::string> Initialize(
			const Framework::GPUDevice& device, Framework::AssetSubsystem* assets);
		void Shutdown() noexcept;
		void Reset() noexcept;
		[[nodiscard]] bool Enter(Framework::f32 duration = 0.35f) noexcept;
		[[nodiscard]] bool Exit(Framework::f32 duration = 0.35f) noexcept;
		void Update(Framework::f32 deltaTime);
		[[nodiscard]] std::expected<void, std::string> Render(Framework::GPUCommandBuffer& commandBuffer);

		[[nodiscard]] TransitionState GetCurrentState() const noexcept;
		[[nodiscard]] bool HasRenderedExit() const noexcept;

	private:
		static constexpr DXGI_FORMAT TARGET_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

		std::optional<Framework::GPUGraphicsPipelineState> pipeline;
		TransitionState state = TransitionState::IDLE;
		Framework::f32 opacity = 0.0f;
		Framework::f32 duration = 0.35f;
		bool hasRenderedExit = false;
	};

	inline TransitionState Transition::GetCurrentState() const noexcept
	{
		return state;
	}

	inline bool Transition::HasRenderedExit() const noexcept
	{
		return hasRenderedExit;
	}
}
