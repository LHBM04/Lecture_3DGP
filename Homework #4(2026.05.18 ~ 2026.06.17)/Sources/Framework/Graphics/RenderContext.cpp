#include "Precompiled.h"
#include "RenderContext.h"

#include "Buffer.h"
#include "GraphicsError.h"
#include "Pipeline.h"
#include "SwapChain.h"

namespace TUK::Framework
{
	RenderContext::RenderContext() noexcept
		: commandAllocator()
		, commandList()
		, isRecording(false)
	{
	}

	std::expected<void, std::string> RenderContext::Initialize(ID3D12Device& device)
	{
		if (commandAllocator || commandList)
		{
			return std::unexpected(std::string("RenderContext가 이미 초기화되어 있습니다."));
		}
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> createdAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> createdList;
		if (auto result = CheckHResult(device.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(createdAllocator.GetAddressOf())), "명령 할당자 생성"); !result)
		{
			return result;
		}
		if (auto result = CheckHResult(device.CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			createdAllocator.Get(), nullptr, IID_PPV_ARGS(createdList.GetAddressOf())), "명령 목록 생성"); !result)
		{
			return result;
		}
		if (auto result = CheckHResult(createdList->Close(), "명령 목록 닫기"); !result)
		{
			return result;
		}
		commandAllocator = std::move(createdAllocator);
		commandList = std::move(createdList);
		return {};
	}

	std::expected<void, std::string> RenderContext::Begin()
	{
		assert(commandList && commandAllocator);
		if (isRecording)
		{
			return std::unexpected(std::string("명령 기록을 시작할 수 없는 상태입니다."));
		}
		const auto allocator = CheckHResult(commandAllocator->Reset(), "명령 할당자 초기화");
		if (!allocator)
		{
			return allocator;
		}
		const auto list = CheckHResult(commandList->Reset(commandAllocator.Get(), nullptr), "명령 목록 초기화");
		if (!list)
		{
			return list;
		}
		isRecording = true;
		return {};
	}

	std::expected<void, std::string> RenderContext::End()
	{
		assert(commandList && commandAllocator);
		if (!isRecording)
		{
			return std::unexpected(std::string("기록 중인 명령 목록이 없습니다."));
		}
		const auto result = CheckHResult(commandList->Close(), "명령 목록 닫기");
		if (!result)
		{
			return result;
		}
		isRecording = false;
		return {};
	}

	void RenderContext::SetSwapChain(SwapChain& target)
	{
		assert(commandList && commandAllocator);
		target.Bind(*commandList.Get());
	}

	void RenderContext::ClearSwapChain(SwapChain& target, const std::array<float, 4>& color)
	{
		assert(commandList && commandAllocator);
		target.Clear(*commandList.Get(), color);
	}

	void RenderContext::EndSwapChain(SwapChain& target)
	{
		assert(commandList && commandAllocator);
		target.EndRender(*commandList.Get());
	}

	void RenderContext::SetPipeline(const Pipeline& pipeline)
	{
		assert(commandList && commandAllocator);
		pipeline.Bind(*commandList.Get());
	}

	void RenderContext::SetDescriptorHeaps(std::span<const std::reference_wrapper<ID3D12DescriptorHeap>> heaps)
	{
		assert(commandList && commandAllocator);
		std::vector<ID3D12DescriptorHeap*> nativeHeaps(heaps.size());
		for (std::size_t index = 0; index < heaps.size(); ++index)
		{
			nativeHeaps[index] = &heaps[index].get();
		}
		commandList->SetDescriptorHeaps(static_cast<UINT>(heaps.size()), nativeHeaps.data());
	}

	void RenderContext::SetRootDescriptorTable(UINT parameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE descriptor)
	{
		assert(commandList && commandAllocator);
		commandList->SetGraphicsRootDescriptorTable(parameterIndex, descriptor);
	}

	void RenderContext::SetRootConstantBufferView(UINT parameterIndex, D3D12_GPU_VIRTUAL_ADDRESS address)
	{
		assert(commandList && commandAllocator);
		commandList->SetGraphicsRootConstantBufferView(parameterIndex, address);
	}

	void RenderContext::SetRootShaderResourceView(UINT parameterIndex, D3D12_GPU_VIRTUAL_ADDRESS address)
	{
		assert(commandList && commandAllocator);
		commandList->SetGraphicsRootShaderResourceView(parameterIndex, address);
	}

	void RenderContext::SetRoot32BitConstants(UINT parameterIndex, std::span<const std::uint32_t> values, UINT offset)
	{
		assert(commandList && commandAllocator);
		commandList->SetGraphicsRoot32BitConstants(parameterIndex, static_cast<UINT>(values.size()), values.data(), offset);
	}

	void RenderContext::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology)
	{
		assert(commandList && commandAllocator);
		commandList->IASetPrimitiveTopology(topology);
	}

	void RenderContext::SetVertexBuffers(UINT startSlot, std::span<const D3D12_VERTEX_BUFFER_VIEW> views)
	{
		assert(commandList && commandAllocator);
		commandList->IASetVertexBuffers(startSlot, static_cast<UINT>(views.size()), views.data());
	}

	void RenderContext::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& view)
	{
		assert(commandList && commandAllocator);
		commandList->IASetIndexBuffer(&view);
	}

	void RenderContext::SetViewports(std::span<const D3D12_VIEWPORT> viewports)
	{
		assert(commandList && commandAllocator);
		commandList->RSSetViewports(static_cast<UINT>(viewports.size()), viewports.data());
	}

	void RenderContext::SetScissorRects(std::span<const D3D12_RECT> rectangles)
	{
		assert(commandList && commandAllocator);
		commandList->RSSetScissorRects(static_cast<UINT>(rectangles.size()), rectangles.data());
	}

	void RenderContext::DrawInstanced(UINT vertexCount, UINT instanceCount, UINT startVertex, UINT startInstance)
	{
		assert(commandList && commandAllocator);
		commandList->DrawInstanced(vertexCount, instanceCount, startVertex, startInstance);
	}

	void RenderContext::DrawIndexedInstanced(UINT indexCount, UINT instanceCount, UINT startIndex, INT baseVertex, UINT startInstance)
	{
		assert(commandList && commandAllocator);
		commandList->DrawIndexedInstanced(indexCount, instanceCount, startIndex, baseVertex, startInstance);
	}

	void RenderContext::SetComputeRootDescriptorTable(UINT parameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE descriptor)
	{
		assert(commandList && commandAllocator);
		commandList->SetComputeRootDescriptorTable(parameterIndex, descriptor);
	}

	void RenderContext::SetComputeRootConstantBufferView(UINT parameterIndex, D3D12_GPU_VIRTUAL_ADDRESS address)
	{
		assert(commandList && commandAllocator);
		commandList->SetComputeRootConstantBufferView(parameterIndex, address);
	}

	void RenderContext::SetComputeRootShaderResourceView(UINT parameterIndex, D3D12_GPU_VIRTUAL_ADDRESS address)
	{
		assert(commandList && commandAllocator);
		commandList->SetComputeRootShaderResourceView(parameterIndex, address);
	}

	void RenderContext::SetComputeRootUnorderedAccessView(UINT parameterIndex, D3D12_GPU_VIRTUAL_ADDRESS address)
	{
		assert(commandList && commandAllocator);
		commandList->SetComputeRootUnorderedAccessView(parameterIndex, address);
	}

	void RenderContext::SetComputeRoot32BitConstants(UINT parameterIndex, std::span<const std::uint32_t> values, UINT offset)
	{
		assert(commandList && commandAllocator);
		commandList->SetComputeRoot32BitConstants(parameterIndex, static_cast<UINT>(values.size()), values.data(), offset);
	}

	void RenderContext::Dispatch(UINT groupCountX, UINT groupCountY, UINT groupCountZ)
	{
		assert(commandList && commandAllocator);
		commandList->Dispatch(groupCountX, groupCountY, groupCountZ);
	}

	void RenderContext::ResourceBarriers(std::span<const D3D12_RESOURCE_BARRIER> barriers)
	{
		assert(commandList && commandAllocator);
		commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
	}

	void RenderContext::TransitionBuffer(const Buffer& buffer, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
	{
		assert(commandList && commandAllocator);
		if (before == after)
		{
			return;
		}
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = &buffer.GetResource();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = before;
		barrier.Transition.StateAfter = after;
		commandList->ResourceBarrier(1, &barrier);
	}

	std::expected<void, std::string> RenderContext::CopyBuffer(const Buffer& destination, UINT64 destinationOffset,
		const Buffer& source, UINT64 sourceOffset, UINT64 size)
	{
		assert(commandList && commandAllocator);
		if (!isRecording)
		{
			return std::unexpected(std::string("명령 기록 중에만 버퍼를 복사할 수 있습니다."));
		}
		if (destinationOffset > destination.GetSize() || size > destination.GetSize() - destinationOffset
			|| sourceOffset > source.GetSize() || size > source.GetSize() - sourceOffset)
		{
			return std::unexpected(std::string("버퍼 복사 범위를 벗어났습니다."));
		}
		if (size == 0)
		{
			return {};
		}
		if (&destination.GetResource() == &source.GetResource())
		{
			return std::unexpected(std::string("CopyBuffer에는 서로 다른 버퍼가 필요합니다."));
		}
		commandList->CopyBufferRegion(&destination.GetResource(), destinationOffset,
			&source.GetResource(), sourceOffset, size);
		return {};
	}

	void RenderContext::CopyResource(ID3D12Resource& destination, ID3D12Resource& source)
	{
		assert(commandList && commandAllocator);
		commandList->CopyResource(&destination, &source);
	}

	void RenderContext::Execute(ID3D12CommandQueue& queue)
	{
		assert(commandList && commandAllocator);
		ID3D12CommandList* lists[]{ commandList.Get() };
		queue.ExecuteCommandLists(1, lists);
	}

	bool RenderContext::IsRecording() const noexcept
	{
		return isRecording;
	}

	void RenderContext::Release() noexcept
	{
		isRecording = false;
		commandList.Reset();
		commandAllocator.Reset();
	}
}
