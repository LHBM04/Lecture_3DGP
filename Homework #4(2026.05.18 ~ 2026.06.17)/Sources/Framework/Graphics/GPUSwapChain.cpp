#include "Precompiled.hpp"
#include "GPUSwapChain.hpp"

namespace TUK::Framework
{
	GPUSwapChain::~GPUSwapChain() noexcept
	{
		if (dxgiSwapChain)
		{
			dxgiSwapChain->Release();
			dxgiSwapChain = nullptr;
		}
	}

	IDXGISwapChain4& GPUSwapChain::GetDXGISwapChain() const noexcept
	{
		return *(dxgiSwapChain.Get());
	}

	GPUSwapChain::GPUSwapChain(IDXGISwapChain4* dxgiSwapChain) noexcept
		: dxgiSwapChain(dxgiSwapChain)
	{
	}
}
