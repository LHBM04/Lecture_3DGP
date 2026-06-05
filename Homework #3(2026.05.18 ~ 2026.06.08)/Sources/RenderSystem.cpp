#include "Precompiled.h"
#include "RenderSystem.h"

#include "Logger.h"

bool RenderSystem::Initialize(HWND window_)
{
	assert(window_ != nullptr);

	RECT clientRect{};
	::GetClientRect(window_, &clientRect);
	clientWidth = static_cast<UINT>(clientRect.right - clientRect.left);
	clientHeight = static_cast<UINT>(clientRect.bottom - clientRect.top);

	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(clientWidth);
	viewport.Height = static_cast<float>(clientHeight);
	viewport.MinDepth = D3D12_MIN_DEPTH;
	viewport.MaxDepth = D3D12_MAX_DEPTH;

	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = static_cast<LONG>(clientWidth);
	scissorRect.bottom = static_cast<LONG>(clientHeight);

	UINT factoryFlags{ 0 };
#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(::D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	if (FAILED(::CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory))))
	{
		Logger::Critical(L"Failed to create DXGI Factory.");
		return false;
	}

	if (FAILED(CreateDevice()))
	{
		Logger::Critical(L"Failed to create D3D12 Device.");
		return false;
	}

	if (FAILED(CreateSyncObjects()))
	{
		Logger::Critical(L"Failed to create Sync Objects.");
		return false;
	}

	if (FAILED(CreateCommandObjects()))
	{
		Logger::Critical(L"Failed to create Command Objects.");
		return false;
	}

	if (FAILED(CreateSwapChain(window_)))
	{
		Logger::Critical(L"Failed to create Swap Chain.");
		return false;
	}

	if (FAILED(CreateDescriptorHeaps()))
	{
		Logger::Critical(L"Failed to create Descriptor Heaps.");
		return false;
	}

	if (FAILED(CreateRenderTargetViews()))
	{
		Logger::Critical(L"Failed to create Render Target Views.");
		return false;
	}

	if (FAILED(CreateDepthStencilView()))
	{
		Logger::Critical(L"Failed to create Depth Stencil View.");
		return false;
	}

	if (FAILED(CreateConstantBuffers()))
	{
		Logger::Critical(L"Failed to create Constant Buffers.");
		return false;
	}

	return true;
}

void RenderSystem::Release()
{
	if (device == nullptr)
	{
		return;
	}

	WaitForGpu();

	if (fenceEvent != nullptr)
	{
		::CloseHandle(fenceEvent);
		fenceEvent = nullptr;
	}

	for (auto& backBuffer : backBuffers)
	{
		backBuffer.Reset();
	}

	depthStencilBuffer.Reset();
	swapChain.Reset();
	commandList.Reset();

	for (auto& commandAllocator : commandAllocators)
	{
		commandAllocator.Reset();
	}

	commandQueue.Reset();
	fence.Reset();
	rtvHeap.Reset();
	dsvHeap.Reset();

	for (FrameConstantBuffer& frameConstantBuffer : frameConstantBuffers)
	{
		if (frameConstantBuffer.resource != nullptr && frameConstantBuffer.mappedData != nullptr)
		{
			frameConstantBuffer.resource->Unmap(0, nullptr);
			frameConstantBuffer.mappedData = nullptr;
		}

		frameConstantBuffer.resource.Reset();
		frameConstantBuffer.gpuAddress = 0;
		frameConstantBuffer.currentOffset = 0;
	}

	device.Reset();
	factory.Reset();
}

void RenderSystem::BeginFrame()
{
	assert(commandQueue != nullptr);
	assert(commandList != nullptr);
	assert(commandAllocators[frameIndex] != nullptr);

	commandAllocators[frameIndex]->Reset();
	commandList->Reset(commandAllocators[frameIndex].Get(), nullptr);
	frameConstantBuffers[frameIndex].currentOffset = 0;

	const D3D12_RESOURCE_BARRIER barrier{
		CreateTransitionBarrier(
			backBuffers[frameIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET)
	};
	commandList->ResourceBarrier(1, &barrier);

	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);
	commandList->OMSetRenderTargets(1, &rtvHandles[frameIndex], FALSE, &dsvHandle);
}

void RenderSystem::EndFrame()
{
	const D3D12_RESOURCE_BARRIER barrier{
		CreateTransitionBarrier(
			backBuffers[frameIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT)
	};
	commandList->ResourceBarrier(1, &barrier);
	commandList->Close();

	ID3D12CommandList* const commandLists[]{ commandList.Get() };
	commandQueue->ExecuteCommandLists(1, commandLists);
}

void RenderSystem::Clear()
{
	static constexpr float ClearColor[4]{ 0.1f, 0.2f, 0.35f, 1.0f };

	commandList->ClearRenderTargetView(rtvHandles[frameIndex], ClearColor, 0, nullptr);
	commandList->ClearDepthStencilView(
		dsvHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f,
		0,
		0,
		nullptr);
}

void RenderSystem::Present()
{
	swapChain->Present(1, 0);
	MoveToNextFrame();
}

ID3D12Device* RenderSystem::GetDevice() const noexcept
{
	return device.Get();
}

ID3D12GraphicsCommandList* RenderSystem::GetCommandList() const noexcept
{
	return commandList.Get();
}

void RenderSystem::WaitForGpu()
{
	if (commandQueue == nullptr || fence == nullptr || fenceEvent == nullptr)
	{
		return;
	}

	const UINT64 signalValue{ ++fenceValues[frameIndex] };
	commandQueue->Signal(fence.Get(), signalValue);

	if (fence->GetCompletedValue() < signalValue)
	{
		fence->SetEventOnCompletion(signalValue, fenceEvent);
		::WaitForSingleObject(fenceEvent, INFINITE);
	}
}

void RenderSystem::MoveToNextFrame()
{
	const UINT64 currentFenceValue{ ++fenceValues[frameIndex] };
	commandQueue->Signal(fence.Get(), currentFenceValue);

	frameIndex = swapChain->GetCurrentBackBufferIndex();

	if (fence->GetCompletedValue() < fenceValues[frameIndex])
	{
		fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent);
		::WaitForSingleObject(fenceEvent, INFINITE);
	}
}

D3D12_RESOURCE_BARRIER RenderSystem::CreateTransitionBarrier(
	ID3D12Resource* resource_,
	D3D12_RESOURCE_STATES before_,
	D3D12_RESOURCE_STATES after_) const noexcept
{
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = resource_;
	barrier.Transition.StateBefore = before_;
	barrier.Transition.StateAfter = after_;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	return barrier;
}

HRESULT RenderSystem::CreateDevice()
{
	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
	for (UINT adapterIndex{ 0 }; factory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND; ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 adapterDesc{};
		adapter->GetDesc1(&adapterDesc);

		if ((adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0)
		{
			adapter.Reset();
			continue;
		}

		if (SUCCEEDED(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device))))
		{
			break;
		}

		adapter.Reset();
	}

	if (device == nullptr)
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
		if (FAILED(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter))))
		{
			return E_FAIL;
		}

		if (FAILED(::D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device))))
		{
			Logger::Critical(L"Failed to create WARP Device.");
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT RenderSystem::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	if (FAILED(device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue))))
	{
		return E_FAIL;
	}

	for (auto& commandAllocator : commandAllocators)
	{
		if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator))))
		{
			return E_FAIL;
		}
	}

	if (FAILED(device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocators[0].Get(),
		nullptr,
		IID_PPV_ARGS(&commandList))))
	{
		return E_FAIL;
	}

	commandList->Close();
	return S_OK;
}

HRESULT RenderSystem::CreateSwapChain(HWND window_)
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.BufferCount = BackBufferCount;
	swapChainDesc.Width = clientWidth;
	swapChainDesc.Height = clientHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
	if (FAILED(factory->CreateSwapChainForHwnd(
		commandQueue.Get(),
		window_,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain1)))
	{
		return E_FAIL;
	}

	if (FAILED(factory->MakeWindowAssociation(window_, DXGI_MWA_NO_ALT_ENTER)))
	{
		return E_FAIL;
	}

	if (FAILED(swapChain1.As(&swapChain)))
	{
		return E_FAIL;
	}

	frameIndex = swapChain->GetCurrentBackBufferIndex();
	return S_OK;
}

HRESULT RenderSystem::CreateDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.NumDescriptors = BackBufferCount;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if (FAILED(device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&rtvHeap))))
	{
		Logger::Critical(L"Failed to create RTV Heap.");
		return E_FAIL;
	}
	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	descriptorHeapDesc.NumDescriptors = 1;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	if (FAILED(device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&dsvHeap))))
	{
		Logger::Critical(L"Failed to create DSV Heap.");
		return E_FAIL;
	}
	dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	return S_OK;
}

HRESULT RenderSystem::CreateSyncObjects()
{
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msaaQualityLevels{};
	msaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	msaaQualityLevels.SampleCount = 4;
	msaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msaaQualityLevels.NumQualityLevels = 0;
	if (FAILED(device->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msaaQualityLevels,
		sizeof(msaaQualityLevels))))
	{
		return E_FAIL;
	}

	msaa4xQualityLevels = msaaQualityLevels.NumQualityLevels;
	isEnableMsaa4x = msaa4xQualityLevels > 1;

	if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
	{
		return E_FAIL;
	}

	fenceValues.fill(0);
	fenceEvent = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);
	return (fenceEvent != nullptr) ? S_OK : E_FAIL;
}

HRESULT RenderSystem::CreateConstantBuffers()
{
	for (FrameConstantBuffer& frameConstantBuffer : frameConstantBuffers)
	{
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Width = FrameConstantBufferSize;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		if (FAILED(device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&frameConstantBuffer.resource))))
		{
			return E_FAIL;
		}

		if (FAILED(frameConstantBuffer.resource->Map(0, nullptr, reinterpret_cast<void**>(&frameConstantBuffer.mappedData))))
		{
			return E_FAIL;
		}

		frameConstantBuffer.gpuAddress = frameConstantBuffer.resource->GetGPUVirtualAddress();
		frameConstantBuffer.currentOffset = 0;
	}

	return S_OK;
}

HRESULT RenderSystem::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	for (UINT bufferIndex{ 0 }; bufferIndex < BackBufferCount; ++bufferIndex)
	{
		if (FAILED(swapChain->GetBuffer(bufferIndex, IID_PPV_ARGS(&backBuffers[bufferIndex]))))
		{
			Logger::Critical(L"Failed to get swap chain buffer.");
			return E_FAIL;
		}

		device->CreateRenderTargetView(backBuffers[bufferIndex].Get(), nullptr, rtvHandle);
		rtvHandles[bufferIndex] = rtvHandle;
		rtvHandle.ptr += rtvDescriptorSize;
	}

	return S_OK;
}

HRESULT RenderSystem::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC depthStencilDesc{};
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Width = clientWidth;
	depthStencilDesc.Height = clientHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	if (FAILED(device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(&depthStencilBuffer))))
	{
		Logger::Critical(L"Failed to create Depth Buffer.");
		return E_FAIL;
	}

	dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, dsvHandle);
	return S_OK;
}

D3D12_GPU_VIRTUAL_ADDRESS RenderSystem::UploadConstantData(const void* data_, UINT sizeInBytes_)
{
	FrameConstantBuffer& frameConstantBuffer{ frameConstantBuffers[frameIndex] };
	const UINT alignedSize{ AlignConstantBufferSize(sizeInBytes_) };
	if (frameConstantBuffer.mappedData == nullptr ||
		frameConstantBuffer.currentOffset + alignedSize > FrameConstantBufferSize)
	{
		return 0;
	}

	std::memcpy(frameConstantBuffer.mappedData + frameConstantBuffer.currentOffset, data_, sizeInBytes_);
	const D3D12_GPU_VIRTUAL_ADDRESS gpuAddress{ frameConstantBuffer.gpuAddress + frameConstantBuffer.currentOffset };
	frameConstantBuffer.currentOffset += alignedSize;
	return gpuAddress;
}

UINT RenderSystem::AlignConstantBufferSize(UINT sizeInBytes_) noexcept
{
	return (sizeInBytes_ + 255u) & ~255u;
}
