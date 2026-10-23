#include "Precompiled.hpp"
#include "GPUSwapChain.hpp"

namespace TUK::Framework
{
	ID3D12Resource& GPUSwapChain::GetD3DBackBuffer(UINT index) const
	{
		return renderTargets.at(index).GetD3DResource();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GPUSwapChain::GetD3DRTVHandle(UINT index) const
	{
		return renderTargets.at(index).GetRTVHandle();
	}

	void GPUSwapChain::Present() const
	{
		dxgiSwapChain->Present(1, 0);
	}

	IDXGISwapChain4& GPUSwapChain::GetDXGISwapChain() const noexcept
	{
		return *dxgiSwapChain.Get();
	}

	GPUSwapChain::GPUSwapChain(
		Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain,
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap,
		std::vector<GPUTarget> renderTargets) noexcept
		: dxgiSwapChain(std::move(dxgiSwapChain))
		, rtvHeap(std::move(rtvHeap))
		, renderTargets(std::move(renderTargets))
	{
	}
}
