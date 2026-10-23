#include "Precompiled.hpp"
#include "GPUCommandBuffer.hpp"

#include "GPUTarget.hpp"
#include "GPUTexture.hpp"

namespace TUK::Framework
{
	void GPUCommandBuffer::TransitionTexture(
		const GPUTexture& texture,
		D3D12_RESOURCE_STATES stateBefore,
		D3D12_RESOURCE_STATES stateAfter)
	{
		if (stateBefore == stateAfter)
		{
			return;
		}

		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = &texture.GetD3DResource();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = stateBefore;
		barrier.Transition.StateAfter = stateAfter;
		d3dCommandList->ResourceBarrier(1, &barrier);
	}

	void GPUCommandBuffer::ClearGPUTarget(const GPUTarget& target, const ColorRGBA<float>& clearColor)
	{
		const FLOAT color[] =
		{
			clearColor.GetR(),
			clearColor.GetG(),
			clearColor.GetB(),
			clearColor.GetA()
		};

		d3dCommandList->ClearRenderTargetView(target.GetD3DRTVHandle(), color, 0, nullptr);
	}

	void GPUCommandBuffer::SetGPUTarget(const GPUTarget& target)
	{
		const D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = target.GetD3DRTVHandle();
		d3dCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	}

	ID3D12GraphicsCommandList10& GPUCommandBuffer::GetD3DCommandList() const noexcept
	{
		return *(d3dCommandList.Get());
	}

	ID3D12CommandAllocator& GPUCommandBuffer::GetD3DCommandAllocator() const noexcept
	{
		return *(d3dCommandAllocator.Get());
	}

	GPUCommandBuffer::GPUCommandBuffer(
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> d3dCommandList,
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> d3dCommandAllocator) noexcept
		: d3dCommandList(std::move(d3dCommandList))
		, d3dCommandAllocator(std::move(d3dCommandAllocator))
	{
	}
}
