#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <dxgi1_6.h>

#include "Framework/Graphics/RenderTarget.hpp"
#include "Framework/Graphics/SwapChain.hpp"

namespace TUK::Framework
{
	class SwapChain_Impl : public SwapChain
	{
	public:
		explicit SwapChain_Impl(IDXGISwapChain4* dxgiSwapChain) noexcept;
		~SwapChain_Impl() noexcept override;

		void Clear(CommandBuffer& commandBuffer) const override;
		void Present() const override;

		[[nodiscard]] Vector2D<std::int32_t> GetSize() const noexcept override;
		void SetSize(const Vector2D<std::int32_t>& size) noexcept override;

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
