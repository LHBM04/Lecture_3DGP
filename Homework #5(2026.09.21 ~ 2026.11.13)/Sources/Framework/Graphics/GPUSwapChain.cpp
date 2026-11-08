#include "Precompiled.hpp"
#include "GPUSwapChain.hpp"

#include "../Utilities/Logger.hpp"

namespace TUK::Framework
{
	void GPUSwapChain::Present() const
	{
		ASSERT_FMT(dxgiSwapChain.Get(), "dxgiSwapChain이 nullptr입니다.");

		dxgiSwapChain->Present(1, 0);
	}

	GPUTarget GPUSwapChain::GetCurrentBackBufferTarget() const noexcept
	{
		ASSERT_FMT(dxgiSwapChain.Get(), "dxgiSwapChain이 nullptr입니다.");
		ASSERT_FMT(rtvHeap.Get(), "rtvHeap이 nullptr입니다.");
		ASSERT_FMT(dsvHeap.Get(), "dsvHeap이 nullptr입니다.");

		const u32 index = dxgiSwapChain->GetCurrentBackBufferIndex();
		const CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
			rtvHeap->GetCPUDescriptorHandleForHeapStart(),
			static_cast<INT>(index),
			rtvDescriptorSize);

		const GPUTexture2D& texture = backBuffers[index];
		return GPUTarget(
			texture,
			rtvHandle,
			dsvHeap->GetCPUDescriptorHandleForHeapStart(),
			texture.GetD3DResource().GetDesc().Format);
	}
}
