#pragma once

namespace TUK::Framework
{
	class CommandBuffer;

	class SwapChain
	{
	public:
		virtual ~SwapChain() noexcept = default;

		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;

		SwapChain(SwapChain&&) = default;
		SwapChain& operator=(SwapChain&&) = default;

		virtual void Clear(CommandBuffer& commandBuffer) const = 0;
		virtual void Present() const = 0;

	protected:
		SwapChain() noexcept = default;
	};
}
