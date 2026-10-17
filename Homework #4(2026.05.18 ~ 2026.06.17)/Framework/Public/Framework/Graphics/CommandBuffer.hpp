#pragma once

#include <functional>
#include <span>

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
		virtual ~CommandBuffer() noexcept;

		CommandBuffer(const CommandBuffer&) = delete;
		CommandBuffer& operator=(const CommandBuffer&) = delete;

		CommandBuffer(CommandBuffer&&) = delete;
		CommandBuffer& operator=(CommandBuffer&&) = delete;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void SetRenderTarget(std::reference_wrapper<RenderTarget> renderTarget) = 0;
		virtual void SetRenderTargets(std::span<std::reference_wrapper<RenderTarget>> renderTargets) = 0;

		virtual void ClearRenderTarget(const ColorRGB<float>& clearColor) const = 0;
		virtual void ClearRenderTarget(const ColorRGBA<float>& clearColor) const = 0;

	protected:
		CommandBuffer() noexcept;
	};
}
