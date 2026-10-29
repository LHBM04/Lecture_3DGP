#include "Precompiled.hpp"
#include "GPUSwapChain.hpp"

namespace TUK::Framework
{
	void GPUSwapChain::Present() const
	{
		dxgiSwapChain->Present(1, 0);
	}

	GPUTarget GPUSwapChain::GetCurrentBackBufferTarget() const noexcept
	{
		const std::uint32_t index = dxgiSwapChain->GetCurrentBackBufferIndex();
		D3D12_CPU_DESCRIPTOR_HANDLE rtv = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtv.ptr += static_cast<SIZE_T>(index) * rtvDescriptorSize;

		const GPUTexture& texture = backBuffers[index];
		return GPUTarget(texture, rtv, texture.GetD3DResource().GetDesc().Format);
	}

	IDXGISwapChain4& GPUSwapChain::GetDXGISwapChain() const noexcept
	{
		return *dxgiSwapChain.Get();
	}

	GPUSwapChain::GPUSwapChain(
		Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain,
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap,
		std::vector<GPUTexture> backBuffers,
		std::uint32_t rtvDescriptorSize) noexcept
		: dxgiSwapChain(std::move(dxgiSwapChain))
		, rtvHeap(std::move(rtvHeap))
		, backBuffers(std::move(backBuffers))
		, rtvDescriptorSize(rtvDescriptorSize)
	{
	}
}
