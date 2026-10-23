#include "Precompiled.hpp"
#include "GPUCommandBuffer.hpp"

#include <stdexcept>

namespace TUK::Framework
{
	void GPUCommandBuffer::BeginRenderPass()
	{
		D3D12_RENDER_PASS_RENDER_TARGET_DESC renderTargetDesc{};
		renderTargetDesc.BeginningAccess.

		D3D12_RENDER_PASS_DEPTH_STENCIL_DESC depthStencilDesc{};

		d3dCommandList->BeginRenderPass(
			1,
			&renderTargetDesc,
			&depthStencilDesc,
			D3D12_RENDER_PASS_FLAG_NONE);
	}

	void GPUCommandBuffer::EndRenderPass()
	{
		d3dCommandList->EndRenderPass();
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
		ID3D12GraphicsCommandList10* d3dCommandList,
		ID3D12CommandAllocator* d3dCommandAllocator) noexcept
		: d3dCommandList(d3dCommandList)
		, d3dCommandAllocator(d3dCommandAllocator)
	{
	}
}
