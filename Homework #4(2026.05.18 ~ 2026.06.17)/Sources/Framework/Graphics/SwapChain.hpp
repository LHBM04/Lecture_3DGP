#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <dxgi1_6.h>

#include "Framework/Math/Vector2D.hpp"
#include "Framework/Graphics/RenderTarget.hpp"

namespace TUK::Framework
{
	class CommandBuffer;

	class SwapChain
	{
	public:
		explicit SwapChain(IDXGISwapChain4* dxgiSwapChain) noexcept;
		~SwapChain() noexcept;

		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;

		SwapChain(SwapChain&&) = delete;
		SwapChain& operator=(SwapChain&&) = delete;

		void Clear(CommandBuffer& commandBuffer) const;
		void Present() const;

		[[nodiscard]] Vector2D<std::int32_t> GetSize() const noexcept;
		void SetSize(const Vector2D<std::int32_t>& size) noexcept;

		/** DXGI 스왑체인 Getter */
		[[nodiscard]] IDXGISwapChain4& GetDXGISwapChain() const noexcept;

	private:
		void CreateRenderTargets();
		void ReleaseRenderTargets() noexcept;

	private:
		IDXGISwapChain4* dxgiSwapChain;
		std::vector<std::unique_ptr<RenderTarget>> renderTargets;
	};
}
