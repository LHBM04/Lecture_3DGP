#include "Precompiled.hpp"
#include "D3DSwapChain.hpp"

namespace TUK::Framework
{
	void D3DSwapChain::Present() const
	{
		dxgiSwapChain->Present(1, 0);
	}

	D3DTarget D3DSwapChain::GetCurrentBackBufferTarget() const noexcept
	{
		const std::uint32_t index = dxgiSwapChain->GetCurrentBackBufferIndex();
		D3D12_CPU_DESCRIPTOR_HANDLE rtv = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtv.ptr += static_cast<SIZE_T>(index) * rtvDescriptorSize;

		const D3DTexture& texture = backBuffers[index];
		return D3DTarget(texture, rtv, texture.GetD3DResource().GetDesc().Format);
	}

	IDXGISwapChain4& D3DSwapChain::GetDXGISwapChain() const noexcept
	{
		return *dxgiSwapChain.Get();
	}

	D3DSwapChain::D3DSwapChain(
		Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain,
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap,
		std::vector<D3DTexture> backBuffers,
		std::uint32_t rtvDescriptorSize) noexcept
		: dxgiSwapChain(std::move(dxgiSwapChain))
		, rtvHeap(std::move(rtvHeap))
		, backBuffers(std::move(backBuffers))
		, rtvDescriptorSize(rtvDescriptorSize)
	{
	}
}
