#include "Precompiled.hpp"
#include "GPUSwapChain.hpp"

namespace TUK::Framework
{
	IDXGISwapChain4& GPUSwapChain::GetDXGISwapChain() const noexcept
	{
		return *(dxgiSwapChain.Get());
	}

	GPUSwapChain::GPUSwapChain(IDXGISwapChain4* dxgiSwapChain) noexcept
		: dxgiSwapChain(dxgiSwapChain)
	{
	}
}
