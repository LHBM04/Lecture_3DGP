#include "Precompiled.h"
#include "RenderContext.h"
#include "SwapChain.h"

#include <cassert>
#include <vector>

namespace
{
	std::expected<void, std::string> CheckCommandResult(HRESULT result, std::string_view operation)
	{
		if (FAILED(result))
		{
			return std::unexpected(std::format("RenderContext: {} 실패 (HRESULT: 0x{:08X}).",
				operation, static_cast<unsigned long>(result)));
		}
		return {};
	}
}

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
			return std::unexpected(std::string{ "RenderContext가 이미 초기화되어 있습니다." });
		}
		const auto allocator = CheckCommandResult(device.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(commandAllocator.GetAddressOf())), "명령 할당자 생성");
		if (!allocator)
		{
			return allocator;
		}
		const auto list = CheckCommandResult(device.CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			commandAllocator.Get(), nullptr, IID_PPV_ARGS(commandList.GetAddressOf())), "명령 목록 생성");
		if (!list)
		{
			return list;
		}
		return CheckCommandResult(commandList->Close(), "명령 목록 닫기");
	}

	std::expected<void, std::string> RenderContext::Begin()
	{
		assert(commandList && commandAllocator);
		if (isRecording)
		{
			return std::unexpected(std::string{ "명령 기록을 시작할 수 없는 상태입니다." });
		}
		const auto allocator = CheckCommandResult(commandAllocator->Reset(), "명령 할당자 초기화");
		if (!allocator)
		{
			return allocator;
		}
		const auto list = CheckCommandResult(commandList->Reset(commandAllocator.Get(), nullptr), "명령 목록 초기화");
		if (!list)
		{
			return list;
		}
		isRecording = true;
		return {};
	}

	std::expected<void, std::string> RenderContext::End()
	{
		AssertInitialized();
		if (!isRecording)
		{
			return std::unexpected(std::string{ "기록 중인 명령 목록이 없습니다." });
		}
		const auto result = CheckCommandResult(commandList->Close(), "명령 목록 닫기");
		if (!result)
		{
			return result;
		}
		isRecording = false;
		return {};
	}

	std::expected<void, std::string> RenderContext::SetSwapChain(SwapChain& target)
	{
		AssertInitialized();
		if (!isRecording)
		{
			return std::unexpected(std::string{ "명령 기록 중에만 렌더 타깃을 설정할 수 있습니다." });
		}
		target.Bind(*commandList.Get());
		return {};
	}

	std::expected<void, std::string> RenderContext::ClearSwapChain(SwapChain& target, const std::array<float, 4>& color)
	{
		AssertInitialized();
		if (!isRecording)
		{
			return std::unexpected(std::string{ "명령 기록 중에만 렌더 타깃을 클리어할 수 있습니다." });
		}
		target.Clear(*commandList.Get(), color);
		return {};
	}

	std::expected<void, std::string> RenderContext::EndSwapChain(SwapChain& target)
	{
		AssertInitialized();
		if (!isRecording)
		{
			return std::unexpected(std::string{ "명령 기록 중에만 렌더 타깃의 기록을 마칠 수 있습니다." });
		}
		target.EndRender(*commandList.Get());
		return {};
	}

	void RenderContext::SetPipelineState(ID3D12PipelineState& pipelineState)
	{
		AssertInitialized();
		commandList->SetPipelineState(&pipelineState);
	}

	void RenderContext::SetRootSignature(ID3D12RootSignature& rootSignature)
	{
		AssertInitialized();
		commandList->SetGraphicsRootSignature(&rootSignature);
	}

	void RenderContext::SetDescriptorHeaps(std::span<const std::reference_wrapper<ID3D12DescriptorHeap>> heaps)
	{
		AssertInitialized();
		std::vector<ID3D12DescriptorHeap*> nativeHeaps(heaps.size());
		for (std::size_t index = 0; index < heaps.size(); ++index)
		{
			nativeHeaps[index] = &heaps[index].get();
		}
		commandList->SetDescriptorHeaps(static_cast<UINT>(heaps.size()), nativeHeaps.data());
	}

	void RenderContext::SetRootDescriptorTable(UINT parameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE descriptor)
	{
		AssertInitialized();
		commandList->SetGraphicsRootDescriptorTable(parameterIndex, descriptor);
	}

	void RenderContext::SetRootConstantBufferView(UINT parameterIndex, D3D12_GPU_VIRTUAL_ADDRESS address)
	{
		AssertInitialized();
		commandList->SetGraphicsRootConstantBufferView(parameterIndex, address);
	}

	void RenderContext::SetRootShaderResourceView(UINT parameterIndex, D3D12_GPU_VIRTUAL_ADDRESS address)
	{
		AssertInitialized();
		commandList->SetGraphicsRootShaderResourceView(parameterIndex, address);
	}

	void RenderContext::SetRoot32BitConstants(UINT parameterIndex, std::span<const std::uint32_t> values, UINT offset)
	{
		AssertInitialized();
		commandList->SetGraphicsRoot32BitConstants(parameterIndex, static_cast<UINT>(values.size()), values.data(), offset);
	}

	void RenderContext::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology)
	{
		AssertInitialized();
		commandList->IASetPrimitiveTopology(topology);
	}

	void RenderContext::SetVertexBuffers(UINT startSlot, std::span<const D3D12_VERTEX_BUFFER_VIEW> views)
	{
		AssertInitialized();
		commandList->IASetVertexBuffers(startSlot, static_cast<UINT>(views.size()), views.data());
	}

	void RenderContext::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& view)
	{
		AssertInitialized();
		commandList->IASetIndexBuffer(&view);
	}

	void RenderContext::SetViewports(std::span<const D3D12_VIEWPORT> viewports)
	{
		AssertInitialized();
		commandList->RSSetViewports(static_cast<UINT>(viewports.size()), viewports.data());
	}

	void RenderContext::SetScissorRects(std::span<const D3D12_RECT> rectangles)
	{
		AssertInitialized();
		commandList->RSSetScissorRects(static_cast<UINT>(rectangles.size()), rectangles.data());
	}

	void RenderContext::DrawInstanced(UINT vertexCount, UINT instanceCount, UINT startVertex, UINT startInstance)
	{
		AssertInitialized();
		commandList->DrawInstanced(vertexCount, instanceCount, startVertex, startInstance);
	}

	void RenderContext::DrawIndexedInstanced(UINT indexCount, UINT instanceCount, UINT startIndex, INT baseVertex, UINT startInstance)
	{
		AssertInitialized();
		commandList->DrawIndexedInstanced(indexCount, instanceCount, startIndex, baseVertex, startInstance);
	}

	void RenderContext::ResourceBarriers(std::span<const D3D12_RESOURCE_BARRIER> barriers)
	{
		AssertInitialized();
		commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
	}

	void RenderContext::CopyResource(ID3D12Resource& destination, ID3D12Resource& source)
	{
		AssertInitialized();
		commandList->CopyResource(&destination, &source);
	}

	std::expected<void, std::string> RenderContext::Execute(ID3D12CommandQueue& queue)
	{
		AssertInitialized();
		if (!commandList || isRecording)
		{
			return std::unexpected(std::string{ "기록을 마친 명령 목록만 제출할 수 있습니다." });
		}
		ID3D12CommandList* lists[] = { commandList.Get() };
		queue.ExecuteCommandLists(1, lists);
		return {};
	}

	void RenderContext::AssertInitialized() const noexcept
	{
		assert(commandList && commandAllocator);
	}

	bool RenderContext::IsRecording() const noexcept
	{
		return isRecording;
	}

	void RenderContext::Shutdown() noexcept
	{
		isRecording = false;
		commandList.Reset();
		commandAllocator.Reset();
	}
}
