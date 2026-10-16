#include "Precompiled.hpp"
#include "SwapChain_Impl.hpp"

#include "Framework/Graphics/CommandBuffer.hpp"

namespace TUK::Framework
{
	SwapChain_Impl::SwapChain_Impl(IDXGISwapChain4* dxgiSwapChain) noexcept
		: dxgiSwapChain(dxgiSwapChain)
	{
		assert(dxgiSwapChain);
	}

	SwapChain_Impl::~SwapChain_Impl() noexcept
	{
		if (dxgiSwapChain)
		{
			dxgiSwapChain->Release();
			dxgiSwapChain = nullptr;
		}
	}

	void SwapChain_Impl::Clear(CommandBuffer& commandBuffer) const
	{
		assert(dxgiSwapChain);
		commandBuffer.ClearRenderTarget(ColorRGBA<float>(0, 0, 0, 0));
	}

	void SwapChain_Impl::Present() const
	{
		assert(dxgiSwapChain);
		dxgiSwapChain->Present(1, 0);
	}

	IDXGISwapChain4& SwapChain_Impl::GetDXGISwapChain() const noexcept
	{
		assert(dxgiSwapChain);
		return *dxgiSwapChain;
	}
}
