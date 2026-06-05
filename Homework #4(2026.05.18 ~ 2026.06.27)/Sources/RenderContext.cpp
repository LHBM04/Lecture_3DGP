#include "Precompiled.h"
#include "RenderContext.h"

#include "UploadBuffer.h"

bool RenderContext::Initialize(const RenderContextOptions& options_)
{
	if (options_.device == nullptr || options_.backBufferCount == 0)
	{
		return false;
	}

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.NodeMask = 0;

	if (FAILED(options_.device->CreateCommandQueue(
		&commandQueueDesc,
		IID_PPV_ARGS(commandQueue.GetAddressOf()))))
	{
		return false;
	}

	commandAllocators.resize(options_.backBufferCount);
	for (Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& allocator : commandAllocators)
	{
		if (FAILED(options_.device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(allocator.GetAddressOf()))))
		{
			return false;
		}
	}

	if (FAILED(options_.device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocators[0].Get(),
		nullptr,
		IID_PPV_ARGS(commandList.GetAddressOf()))))
	{
		return false;
	}

	commandList->Close();

	if (FAILED(options_.device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()))))
	{
		return false;
	}

	fenceValues.assign(options_.backBufferCount, 0);
	fenceEvent = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);
	return fenceEvent != nullptr;
}

void RenderContext::Release()
{
	if (fenceEvent != nullptr)
	{
		::CloseHandle(fenceEvent);
		fenceEvent = nullptr;
	}

	fenceValues.clear();
	fence.Reset();
	commandList.Reset();
	commandAllocators.clear();
	commandQueue.Reset();
}

void RenderContext::Reset(UINT frameIndex_)
{
	commandAllocators[frameIndex_]->Reset();
	commandList->Reset(commandAllocators[frameIndex_].Get(), nullptr);
}

void RenderContext::Execute()
{
	commandList->Close();
	ID3D12CommandList* commandLists[]{ commandList.Get() };
	commandQueue->ExecuteCommandLists(static_cast<UINT>(std::size(commandLists)), commandLists);
}

void RenderContext::SetUploadBuffer(UploadBuffer& uploadBuffer_) noexcept
{
	uploadBuffer = &uploadBuffer_;
}

void RenderContext::SetRenderTargetHandles(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_) noexcept
{
	rtvHandle = rtvHandle_;
	dsvHandle = dsvHandle_;
}

void RenderContext::MoveToNextFrame(UINT& frameIndex_, IDXGISwapChain3& swapChain_)
{
	const UINT64 currentFenceValue{ fenceValues[frameIndex_] + 1 };
	commandQueue->Signal(fence.Get(), currentFenceValue);
	fenceValues[frameIndex_] = currentFenceValue;

	frameIndex_ = swapChain_.GetCurrentBackBufferIndex();

	if (fence->GetCompletedValue() < fenceValues[frameIndex_])
	{
		fence->SetEventOnCompletion(fenceValues[frameIndex_], fenceEvent);
		::WaitForSingleObject(fenceEvent, INFINITE);
	}
}

void RenderContext::WaitForGpu(UINT frameIndex_)
{
	const UINT64 fenceValue{ fenceValues[frameIndex_] + 1 };
	commandQueue->Signal(fence.Get(), fenceValue);
	fenceValues[frameIndex_] = fenceValue;

	fence->SetEventOnCompletion(fenceValue, fenceEvent);
	::WaitForSingleObject(fenceEvent, INFINITE);
}

ID3D12CommandQueue& RenderContext::GetCommandQueue() const noexcept
{
	assert(commandQueue != nullptr);
	return *commandQueue.Get();
}

ID3D12GraphicsCommandList& RenderContext::GetCommandList() const noexcept
{
	assert(commandList != nullptr);
	return *commandList.Get();
}

UploadBuffer& RenderContext::GetUploadBuffer() const noexcept
{
	assert(uploadBuffer != nullptr);
	return *uploadBuffer;
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderContext::GetRtvHandle() const noexcept
{
	assert(rtvHandle.ptr != 0);
	return rtvHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderContext::GetDsvHandle() const noexcept
{
	assert(dsvHandle.ptr != 0);
	return dsvHandle;
}
