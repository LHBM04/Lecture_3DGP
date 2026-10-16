#ifndef TUK_GRAPHICS_SWAPCHAINIMPL
#define TUK_GRAPHICS_SWAPCHAINIMPL

#include <dxgi1_6.h>

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

		/** DXGI 스왑체인 Getter */
		[[nodiscard]] IDXGISwapChain4& GetDXGISwapChain() const noexcept;

	private:
		IDXGISwapChain4* dxgiSwapChain;
	};
}

#endif // !TUK_GRAPHICS_SWAPCHAINIMPL
