#include "Precompiled.hpp"
#include "GPUCommandBuffer.hpp"

#include <stdexcept>

namespace TUK::Framework
{
	GPUCommandBuffer::~GPUCommandBuffer() noexcept = default;

	void GPUCommandBuffer::BeginRenderPass(
		std::span<const D3D12_RENDER_PASS_RENDER_TARGET_DESC> renderTargets,
		const D3D12_RENDER_PASS_DEPTH_STENCIL_DESC* depthStencil)
	{
		d3dCommandList->BeginRenderPass(
			static_cast<UINT>(renderTargets.size()),
			renderTargets.empty() ? nullptr : renderTargets.data(),
			depthStencil,
			D3D12_RENDER_PASS_FLAG_NONE);
		isRenderPassActive = true;
	}

	void GPUCommandBuffer::EndRenderPass()
	{
		d3dCommandList->EndRenderPass();
		isRenderPassActive = false;
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
