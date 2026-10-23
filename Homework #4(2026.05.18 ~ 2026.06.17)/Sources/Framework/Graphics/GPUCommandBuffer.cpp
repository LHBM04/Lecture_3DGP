#include "Precompiled.hpp"
#include "GPUCommandBuffer.hpp"

#include <algorithm>
#include <stdexcept>

#include "GPUSwapChain.hpp"

namespace TUK::Framework
{
	void GPUCommandBuffer::BeginRenderPass(const GPUSwapChain& swapChain)
	{
		const UINT index = swapChain.GetDXGISwapChain().GetCurrentBackBufferIndex();
		ID3D12Resource& backBuffer = swapChain.GetD3DBackBuffer(index);

		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = &backBuffer;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		d3dCommandList->ResourceBarrier(1, &barrier);

		D3D12_RENDER_PASS_RENDER_TARGET_DESC description{};
		description.cpuDescriptor = swapChain.GetD3DRTVHandle(index);
		description.BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
		description.BeginningAccess.Clear.ClearValue.Format = backBuffer.GetDesc().Format;
		description.BeginningAccess.Clear.ClearValue.Color[0] = 0.0f;
		description.BeginningAccess.Clear.ClearValue.Color[1] = 0.0f;
		description.BeginningAccess.Clear.ClearValue.Color[2] = 0.0f;
		description.BeginningAccess.Clear.ClearValue.Color[3] = 1.0f;
		description.EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
		d3dCommandList->BeginRenderPass(1, &description, nullptr, D3D12_RENDER_PASS_FLAG_NONE);

		activeRenderTarget = &backBuffer;
	}

	void GPUCommandBuffer::EndRenderPass()
	{
		d3dCommandList->EndRenderPass();

		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = activeRenderTarget.Get();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		d3dCommandList->ResourceBarrier(1, &barrier);

		activeRenderTarget.Reset();
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
