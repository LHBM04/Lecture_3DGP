#pragma once

#include "../Math/Vector2D.hpp"

namespace TUK::Framework
{
	class CommandBuffer;

	class SwapChain
	{
	public:
		virtual ~SwapChain() noexcept;

		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;

		SwapChain(SwapChain&&) = delete;
		SwapChain& operator=(SwapChain&&) = delete;

		virtual void Clear(CommandBuffer& commandBuffer) const = 0;
		virtual void Present() const = 0;

		[[nodiscard]] virtual Vector2D<int> GetSize() const noexcept = 0;
		virtual void SetSize(const Vector2D<int>& size) noexcept = 0;

	protected:
		SwapChain() noexcept;
	};
}
