#include "Precompiled.hpp"
#include "GPUCommandBuffer.hpp"

#include "GPUBuffer.hpp"
#include "GPUGraphicsPipelineState.hpp"
#include "GPUMesh.hpp"
#include "GPURootSignature.hpp"
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

	void GPUCommandBuffer::TransitionBuffer(
		const GPUBuffer& buffer,
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

	void GPUCommandBuffer::CopyGPUBuffer(
		const GPUBuffer& destination,
		std::uint64_t destinationOffset,
		const GPUBuffer& source,
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

	void GPUCommandBuffer::SetGPUGraphicsRootSignature(const GPURootSignature& rootSignature)
	{
		d3dCommandList->SetGraphicsRootSignature(&rootSignature.GetD3DRootSignature());
	}

	void GPUCommandBuffer::SetGPUGraphicsPipelineState(const GPUGraphicsPipelineState& pipelineState)
	{
		d3dCommandList->SetPipelineState(&pipelineState.GetD3DPipelineState());
	}

	void GPUCommandBuffer::SetGPUGraphicsConstantBuffer(std::uint32_t rootParameterIndex, const GPUBuffer& buffer, std::uint64_t offset)
	{
		assert(buffer.GetD3DHeapType() != D3D12_HEAP_TYPE_READBACK);
		assert(offset < buffer.GetSizeInBytes());
		const D3D12_GPU_VIRTUAL_ADDRESS address = buffer.GetGPUVirtualAddress() + offset;
		assert(address % D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT == 0);
		d3dCommandList->SetGraphicsRootConstantBufferView(rootParameterIndex, address);
	}

	void GPUCommandBuffer::SetViewport(const D3D12_VIEWPORT& viewport)
	{
		assert(viewport.Width > 0.0f && viewport.Height > 0.0f);
		d3dCommandList->RSSetViewports(1, &viewport);
	}

	void GPUCommandBuffer::SetScissorRect(const D3D12_RECT& scissorRect)
	{
		d3dCommandList->RSSetScissorRects(1, &scissorRect);
	}

	void GPUCommandBuffer::SetGPUMesh(const GPUMesh& mesh)
	{
		const D3D12_VERTEX_BUFFER_VIEW vertexView = mesh.GetD3DVertexBufferView();
		const auto indexView = mesh.GetD3DIndexBufferView();

		d3dCommandList->IASetPrimitiveTopology(mesh.GetD3DTopology());
		d3dCommandList->IASetVertexBuffers(0, 1, &vertexView);
		d3dCommandList->IASetIndexBuffer(indexView ? &indexView.value() : nullptr);
	}

	void GPUCommandBuffer::Draw(std::uint32_t vertexCount, std::uint32_t firstVertex)
	{
		d3dCommandList->DrawInstanced(vertexCount, 1, firstVertex, 0);
	}

	void GPUCommandBuffer::DrawIndexed(std::uint32_t indexCount, std::uint32_t firstIndex, std::int32_t baseVertex)
	{
		d3dCommandList->DrawIndexedInstanced(indexCount, 1, firstIndex, baseVertex, 0);
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
