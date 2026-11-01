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

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(rtvHeap->GetCPUDescriptorHandleForHeapStart());
		rtv.Offset(static_cast<INT>(index), rtvDescriptorSize);

		const GPUTexture& texture = backBuffers[index];

		GPUTargetProperties properties{};
		properties.texture = &texture;
		properties.rtvHandle = rtv;
		properties.format = texture.GetD3DResource().GetDesc().Format;
		return GPUTarget(properties);
	}
}
