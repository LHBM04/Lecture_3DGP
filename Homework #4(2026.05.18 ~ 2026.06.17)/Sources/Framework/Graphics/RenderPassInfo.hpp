#pragma once

#include <cstdint>
#include <functional>
#include <span>

#include "../Math/ColorRGBA.hpp"

namespace TUK::Framework
{
	class RenderTarget;

	enum class RenderPassBeginningOperation : std::uint8_t
	{
		Preserve,
		Clear,
		Discard,
	};

	enum class RenderPassEndingOperation : std::uint8_t
	{
		Preserve,
		Discard,
	};

	enum class RenderPassState : std::uint8_t
	{
		Present,
		RenderTarget,
		ShaderResource,
		CopySource,
		CopyDestination,
		Common,
	};

	struct RenderPassInfo final
	{
		std::span<const std::reference_wrapper<RenderTarget>> renderTargets;
		ColorRGBA<float> clearColor{ 0.0f, 0.0f, 0.0f, 1.0f };
		RenderPassBeginningOperation operationBefore = RenderPassBeginningOperation::Preserve;
		RenderPassEndingOperation operationAfter = RenderPassEndingOperation::Preserve;
		RenderPassState stateBefore = RenderPassState::Present;
		RenderPassState stateAfter = RenderPassState::Present;
	};
}
