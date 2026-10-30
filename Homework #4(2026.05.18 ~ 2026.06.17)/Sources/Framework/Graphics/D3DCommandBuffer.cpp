#include "Precompiled.hpp"
#include "D3DCommandBuffer.hpp"

#include "D3DBuffer.hpp"
#include "D3DGraphicsPipelineState.hpp"
#include "D3DMesh.hpp"
#include "D3DRootSignature.hpp"
#include "D3DTarget.hpp"
#include "D3DTexture.hpp"

namespace TUK::Framework
{
	void D3DCommandBuffer::TransitionTexture(
		const D3DTexture& texture,
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

	void D3DCommandBuffer::TransitionBuffer(
		const D3DBuffer& buffer,
		D3D12_RESOURCE_STATES stateBefore,
		D3D12_RESOURCE_STATES stateAfter)
	{
		if (stateBefore == stateAfter)
		{
			return;
		}

		assert(buffer.GetD3DHeapType() == D3D12_HEAP_TYPE_DEFAULT);

		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = &buffer.GetD3DResource();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = stateBefore;
		barrier.Transition.StateAfter = stateAfter;
		d3dCommandList->ResourceBarrier(1, &barrier);
	}

	void D3DCommandBuffer::CopyD3DBuffer(
		const D3DBuffer& destination,
		std::uint64_t destinationOffset,
		const D3DBuffer& source,
		std::uint64_t sourceOffset,
		std::uint64_t sizeInBytes)
	{
		assert(&destination.GetD3DResource() != &source.GetD3DResource());
		assert(destination.GetD3DHeapType() != D3D12_HEAP_TYPE_UPLOAD);
		assert(source.GetD3DHeapType() != D3D12_HEAP_TYPE_READBACK);
		assert(destinationOffset <= destination.GetSizeInBytes());
		assert(sizeInBytes <= destination.GetSizeInBytes() - destinationOffset);
		assert(sourceOffset <= source.GetSizeInBytes());
		assert(sizeInBytes <= source.GetSizeInBytes() - sourceOffset);

		if (sizeInBytes == 0)
		{
			return;
		}

		d3dCommandList->CopyBufferRegion(
			&destination.GetD3DResource(), destinationOffset,
			&source.GetD3DResource(), sourceOffset, sizeInBytes);
	}

	void D3DCommandBuffer::SetGPUGraphicsRootSignature(const D3DRootSignature& rootSignature)
	{
		d3dCommandList->SetGraphicsRootSignature(&rootSignature.GetD3DRootSignature());
	}

	void D3DCommandBuffer::SetD3DGraphicsPipelineState(const D3DGraphicsPipelineState& pipelineState)
	{
		d3dCommandList->SetPipelineState(&pipelineState.GetD3DPipelineState());
	}

	void D3DCommandBuffer::SetGPUGraphicsConstantBuffer(std::uint32_t rootParameterIndex, const D3DBuffer& buffer, std::uint64_t offset)
	{
		assert(buffer.GetD3DHeapType() != D3D12_HEAP_TYPE_READBACK);
		assert(offset < buffer.GetSizeInBytes());
		const D3D12_GPU_VIRTUAL_ADDRESS address = buffer.GetGPUVirtualAddress() + offset;
		assert(address % D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT == 0);
		d3dCommandList->SetGraphicsRootConstantBufferView(rootParameterIndex, address);
	}

	void D3DCommandBuffer::SetViewport(const D3D12_VIEWPORT& viewport)
	{
		assert(viewport.Width > 0.0f && viewport.Height > 0.0f);
		d3dCommandList->RSSetViewports(1, &viewport);
	}

	void D3DCommandBuffer::SetScissorRect(const D3D12_RECT& scissorRect)
	{
		d3dCommandList->RSSetScissorRects(1, &scissorRect);
	}

	void D3DCommandBuffer::SetD3DMesh(const D3DMesh& mesh)
	{
		const D3D12_VERTEX_BUFFER_VIEW vertexView = mesh.GetD3DVertexBufferView();
		const D3D12_INDEX_BUFFER_VIEW indexView = mesh.GetD3DIndexBufferView();

		d3dCommandList->IASetPrimitiveTopology(mesh.GetD3DTopology());
		d3dCommandList->IASetVertexBuffers(0, 1, &vertexView);
		d3dCommandList->IASetIndexBuffer(&indexView);
	}

	void D3DCommandBuffer::Draw(std::uint32_t vertexCount, std::uint32_t firstVertex)
	{
		d3dCommandList->DrawInstanced(vertexCount, 1, firstVertex, 0);
	}

	void D3DCommandBuffer::DrawIndexed(std::uint32_t indexCount, std::uint32_t firstIndex, std::int32_t baseVertex)
	{
		d3dCommandList->DrawIndexedInstanced(indexCount, 1, firstIndex, baseVertex, 0);
	}

	void D3DCommandBuffer::ClearD3DTarget(const D3DTarget& target, const ColorRGBA<float>& clearColor)
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

	void D3DCommandBuffer::SetD3DTarget(const D3DTarget& target)
	{
		const D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = target.GetD3DRTVHandle();
		d3dCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	}

	ID3D12GraphicsCommandList10& D3DCommandBuffer::GetD3DCommandList() const noexcept
	{
		return *(d3dCommandList.Get());
	}

	ID3D12CommandAllocator& D3DCommandBuffer::GetD3DCommandAllocator() const noexcept
	{
		return *(d3dCommandAllocator.Get());
	}

	D3DCommandBuffer::D3DCommandBuffer(
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> d3dCommandList,
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> d3dCommandAllocator) noexcept
		: d3dCommandList(std::move(d3dCommandList))
		, d3dCommandAllocator(std::move(d3dCommandAllocator))
	{
	}
}
