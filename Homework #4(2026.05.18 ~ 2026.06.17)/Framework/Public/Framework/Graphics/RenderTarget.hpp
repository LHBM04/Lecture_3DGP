#pragma once

namespace TUK::Framework
{
	class RenderTarget
	{
		friend class CommandBuffer;

	public:
		virtual ~RenderTarget() noexcept;

		RenderTarget(const RenderTarget&) = delete;
		RenderTarget& operator=(const RenderTarget&) = delete;

		RenderTarget(RenderTarget&&) = delete;
		RenderTarget& operator=(RenderTarget&&) = delete;

	protected:
		RenderTarget() noexcept;
	};
}
