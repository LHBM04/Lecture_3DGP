#pragma once

#include <span>
#include <type_traits>

#include <d3d12.h>

#include "../Math/ColorRGB.hpp"
#include "../Math/ColorRGBA.hpp"

namespace TUK::Framework
{
	class RenderSubsystem;
	class RenderTarget;
	class Texture;
	class Shader;
	class Buffer;

	class CommandBuffer
	{
		friend class RenderSubsystem;

	public:
		virtual ~CommandBuffer() noexcept = default;

		CommandBuffer(const CommandBuffer&) = delete;
		CommandBuffer& operator=(const CommandBuffer&) = delete;

		/** 이동 금지 */
		CommandBuffer(CommandBuffer&&) = default;
		CommandBuffer& operator=(CommandBuffer&&) = default;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void SetRenderTarget(std::reference_wrapper<RenderTarget> renderTarget) = 0;
		virtual void SetRenderTargets(std::span<std::reference_wrapper<RenderTarget>> renderTargets) = 0;

		virtual void ClearRenderTarget(const ColorRGB<float>& clearColor) const = 0;
		virtual void ClearRenderTarget(const ColorRGBA<float>& clearColor) const = 0;

	protected:
		CommandBuffer() noexcept = default;
	};
}
