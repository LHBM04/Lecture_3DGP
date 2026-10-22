#pragma once

#include <cstdint>
#include <vector>

#include <dxgi1_6.h>

#include "../Math/Vector2D.hpp"

#include "RenderTarget.hpp"

namespace TUK::Framework
{
	class SwapChain
	{
	public:
		explicit SwapChain(IDXGISwapChain4* dxgiSwapChain) noexcept;
		~SwapChain() noexcept;

		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;

		SwapChain(SwapChain&& other) noexcept;
		SwapChain& operator=(SwapChain&& other) noexcept;

		void Present() const;

		[[nodiscard]] RenderTarget& GetCurrentRenderTarget();
		[[nodiscard]] const RenderTarget& GetCurrentRenderTarget() const;

		[[nodiscard]] Vector2D<std::int32_t> GetSize() const noexcept;
		void SetSize(const Vector2D<std::int32_t>& size) noexcept;

		/** DXGI 스왑체인 Getter */
		[[nodiscard]] IDXGISwapChain4& GetDXGISwapChain() const noexcept;

	private:
		void CreateRenderTargets();
		void ReleaseRenderTargets() noexcept;

		IDXGISwapChain4* dxgiSwapChain;

		/** 해당 스왑체인에서 사용할 렌더 타겟들 */
		std::vector<RenderTarget> renderTargets;
	};
}
