#include "Precompiled.h"
#include "RenderSystem.h"

bool RenderSystem::Initialize(HWND window_)
{
	UINT factoryFlags{ 0 };
#if defined(_DEBUG)
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	if (FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory))))
	{
		return false;
	}

	if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device))))
	{
		return false;
	}

	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	if (FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue))))
	{
		return false;
	}

	RECT rect;
	::GetClientRect(window_, &rect);
	const UINT width{ static_cast<UINT>(rect.right - rect.left) };
	const UINT height{ static_cast<UINT>(rect.bottom - rect.top) };

	viewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
	scissorRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
	if (FAILED(factory->CreateSwapChainForHwnd(commandQueue.Get(), window_, &swapChainDesc, nullptr, nullptr, &swapChain1)))
	{
		return false;
	}

	if (FAILED(swapChain1.As(&swapChain)))
	{
		return false;
	}

	frameIndex = swapChain->GetCurrentBackBufferIndex();

	// Render Target View 초기화.
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
		rtvHeapDesc.NumDescriptors = FrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap))))
		{
			return false;
		}

		rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };

		for (UINT n = 0; n < FrameCount; n++)
		{
			if (FAILED(swapChain->GetBuffer(n, IID_PPV_ARGS(&backBuffers[n]))))
			{
				return false;
			}
			device->CreateRenderTargetView(backBuffers[n].Get(), nullptr, rtvHandle);
			rtvHandle.ptr += rtvDescriptorSize;
		}
	}

	// Depth Stencil View 초기화.
	{
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (FAILED(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap))))
		{
			return false;
		}

		dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		D3D12_RESOURCE_DESC depthResourceDesc{};
		depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthResourceDesc.Alignment = 0;
		depthResourceDesc.Width = width;
		depthResourceDesc.Height = height;
		depthResourceDesc.DepthOrArraySize = 1;
		depthResourceDesc.MipLevels = 1;
		depthResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthResourceDesc.SampleDesc.Count = 1;
		depthResourceDesc.SampleDesc.Quality = 0;
		depthResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE depthOptimizedClearValue{};
		depthOptimizedClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
		depthOptimizedClearValue.DepthStencil.Stencil = 0;

		D3D12_HEAP_PROPERTIES heapProps{};
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

		if (FAILED(device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&depthResourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthOptimizedClearValue,
			IID_PPV_ARGS(&depthStencilBuffer))))
		{
			return false;
		}

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsvDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart());
	}

	for (std::size_t count{ 0 }; count < FrameCount; count++)
	{
		if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[count]))))
		{
			return false;
		}
	}

	if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[frameIndex].Get(), nullptr, IID_PPV_ARGS(&commandList))))
	{
		return false;
	}

	if (FAILED(commandList->Close()))
	{
		return false;
	}

	if (FAILED(device->CreateFence(fenceValues[frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
	{
		return false;
	}
	fenceValues[frameIndex]++;

	fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
	if (fenceEvent == nullptr)
	{
		return false;
	}

	WaitForGpu();

	return true;
}

void RenderSystem::Release()
{
	WaitForGpu();

	if (fenceEvent != nullptr)
	{
		CloseHandle(fenceEvent);
		fenceEvent = nullptr;
	}
}

void RenderSystem::BeginFrame()
{
	if (FAILED(commandAllocators[frameIndex]->Reset()))
	{
		return;
	}

	if (FAILED(commandList->Reset(commandAllocators[frameIndex].Get(), pipelineState.Get())))
	{
		return;
	}

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = backBuffers[frameIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	rtvHandle.ptr += static_cast<SIZE_T>(frameIndex) * rtvDescriptorSize;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{ dsvHeap->GetCPUDescriptorHandleForHeapStart() };

	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);

	commandList->SetGraphicsRootSignature(rootSignature.Get());
}

void RenderSystem::EndFrame()
{
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = backBuffers[frameIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);

	if (FAILED(commandList->Close()))
	{
		return;
	}

	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

void RenderSystem::Clear()
{
	assert(true); // TODO: assert 걸어라.

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	rtvHandle.ptr += static_cast<SIZE_T>(frameIndex) * rtvDescriptorSize;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{ dsvHeap->GetCPUDescriptorHandleForHeapStart() };

	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->DrawInstanced(3, 1, 0, 0);
}

void RenderSystem::Present()
{
	assert(swapChain != nullptr);

	if (FAILED(swapChain->Present(1, 0)))
	{
		return;
	}

	MoveToNextFrame();
}

void RenderSystem::SetCamera(Camera* camera_)
{
}

void RenderSystem::SetObject(GameObject* object_)
{
}

void RenderSystem::WaitForGpu()
{
	assert(commandQueue != nullptr);
	assert(fence != nullptr);

	if (FAILED(commandQueue->Signal(fence.Get(), fenceValues[frameIndex])))
	{
		return;
	}

	if (FAILED(fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent)))
	{
		return;
	}
	WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);

	fenceValues[frameIndex]++;
}

void RenderSystem::MoveToNextFrame()
{
	const UINT64 currentFenceValue = fenceValues[frameIndex];
	if (FAILED(commandQueue->Signal(fence.Get(), currentFenceValue)))
	{
		return;
	}

	frameIndex = swapChain->GetCurrentBackBufferIndex();

	if (fence->GetCompletedValue() < fenceValues[frameIndex])
	{
		if (FAILED(fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent)))
		{
			return;
		}
		WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
	}

	fenceValues[frameIndex] = currentFenceValue + 1;
}
