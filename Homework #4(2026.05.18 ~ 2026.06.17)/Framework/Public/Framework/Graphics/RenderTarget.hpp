#pragma once

namespace TUK::Framework
{
	class RenderTarget
	{
		friend class CommandBuffer;

	public:
		virtual ~RenderTarget() noexcept = default;

		RenderTarget(const RenderTarget&) = delete;
		RenderTarget& operator=(const RenderTarget&) = delete;

		RenderTarget(RenderTarget&&) noexcept = default;
		RenderTarget& operator=(RenderTarget&&) noexcept = default;

	private:
		RenderTarget() noexcept = default;
	};
}
