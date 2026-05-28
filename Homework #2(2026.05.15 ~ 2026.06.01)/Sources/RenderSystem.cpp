#include "Precompiled.h"
#include "RenderSystem.h"

#include "Window.h"

namespace
{
	constexpr uint32_t MaxRenderTargets = 16;

	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE OffsetHandle(
		D3D12_CPU_DESCRIPTOR_HANDLE handle_,
		uint32_t index_,
		uint32_t descriptorSize_) noexcept
	{
		handle_.ptr += static_cast<SIZE_T>(index_) * descriptorSize_;
		return handle_;
	}
}

bool RenderSystem::Initialize()
{
	UINT factoryFlags{ 0 };
#if defined(_DEBUG)
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
	{
		debugController->EnableDebugLayer();
		factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	if (FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(factory.GetAddressOf()))))
	{
		return false;
	}

	Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter{ nullptr };
	if (FAILED(factory->EnumWarpAdapter(IID_PPV_ARGS(warpAdapter.GetAddressOf()))))
	{
		return false;
	}

	if (FAILED(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(device.GetAddressOf()))))
	{
		return false;
	}

	const D3D12_COMMAND_QUEUE_DESC queueDesc{
		.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
		.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
		.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
		.NodeMask = 0,
	};
	if (FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(commandQueue.GetAddressOf()))))
	{
		return false;
	}

	frameResources.resize(FrameCount);
	for (FrameResource& frameResource : frameResources)
	{
		if (FAILED(device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(frameResource.commandAllocator.GetAddressOf()))))
		{
			return false;
		}
	}

	if (FAILED(device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		frameResources.front().commandAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(commandList.GetAddressOf()))))
	{
		return false;
	}
	commandList->Close();

	const D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		.NumDescriptors = MaxRenderTargets * RenderTarget::BufferCount,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		.NodeMask = 0,
	};
	if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(rtvHeap.GetAddressOf()))))
	{
		return false;
	}

	const D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		.NumDescriptors = MaxRenderTargets,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		.NodeMask = 0,
	};
	if (FAILED(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(dsvHeap.GetAddressOf()))))
	{
		return false;
	}

	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()))))
	{
		return false;
	}

	fenceEvent = ::CreateEventExW(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
	if (fenceEvent == nullptr)
	{
		return false;
	}

	return true;
}

void RenderSystem::Release()
{
	WaitForGpu();

	renderContexts.clear();
	renderers.clear();
	m_windowTargetPairs.clear();
	frameResources.clear();

	fence.Reset();
	dsvHeap.Reset();
	rtvHeap.Reset();
	commandList.Reset();
	commandQueue.Reset();
	device.Reset();
	factory.Reset();

	if (fenceEvent != nullptr)
	{
		::CloseHandle(fenceEvent);
		fenceEvent = nullptr;
	}

#if defined(_DEBUG)
	debugController.Reset();
#endif
}

void RenderSystem::BeginRender()
{
	FrameResource& frameResource{ frameResources[frameIndex] };
	WaitForFrame(frameResource);

	frameResource.commandAllocator->Reset();
	commandList->Reset(frameResource.commandAllocator.Get(), nullptr);
}

void RenderSystem::EndRender()
{
	commandList->Close();

	ID3D12CommandList* lists[]{ commandList.Get() };
	commandQueue->ExecuteCommandLists(static_cast<UINT>(std::size(lists)), lists);

	if (WindowTargetPair* pair = FindWindowTargetPair(activeWindow);
		pair != nullptr && pair->target.swapChain != nullptr)
	{
		pair->target.swapChain->Present(1, 0);
		pair->target.currentBackBufferIndex = pair->target.swapChain->GetCurrentBackBufferIndex();
	}

	FrameResource& frameResource{ frameResources[frameIndex] };
	const uint64_t signaledFenceValue{ nextFenceValue++ };
	commandQueue->Signal(fence.Get(), signaledFenceValue);
	frameResource.fenceValue = signaledFenceValue;

	frameIndex = (frameIndex + 1) % FrameCount;
}

void RenderSystem::ExecuteRenderer(Window* window_, Renderer* renderer_, const RenderContext& context_)
{
	if (window_ == nullptr || renderer_ == nullptr || commandList == nullptr)
	{
		return;
	}

	WindowTargetPair* pair{ FindWindowTargetPair(window_) };
	if (pair == nullptr)
	{
		return;
	}
	activeWindow = window_;
	RenderTarget& renderTarget{ pair->target };

	for (const CameraRenderDescription& cameraDesc : context_.GetCameraDescriptions())
	{
		renderTarget.currentBackBufferIndex = renderTarget.swapChain->GetCurrentBackBufferIndex();
		ID3D12Resource* backBuffer{ renderTarget.backBuffers[renderTarget.currentBackBufferIndex].Get() };

		const D3D12_RESOURCE_BARRIER toRenderTarget{
			.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
			.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
			.Transition = {
				.pResource = backBuffer,
				.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
				.StateBefore = D3D12_RESOURCE_STATE_PRESENT,
				.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET,
			},
		};
		commandList->ResourceBarrier(1, &toRenderTarget);

		const D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ renderTarget.rtvHandles[renderTarget.currentBackBufferIndex] };
		const D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{ renderTarget.dsvHandle };
		commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

		if (cameraDesc.clearFlags == CameraClearFlags::SolidColor)
		{
			commandList->ClearRenderTargetView(rtvHandle, &cameraDesc.backgroundColor.x, 0, nullptr);
			commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		}
		else if (cameraDesc.clearFlags == CameraClearFlags::OnlyDepth)
		{
			commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		}

		const D3D12_VIEWPORT viewport{
			.TopLeftX = cameraDesc.viewport.topLeftX,
			.TopLeftY = cameraDesc.viewport.topLeftY,
			.Width = cameraDesc.viewport.width,
			.Height = cameraDesc.viewport.height,
			.MinDepth = cameraDesc.viewport.minDepth,
			.MaxDepth = cameraDesc.viewport.maxDepth,
		};
		commandList->RSSetViewports(1, &viewport);

		const D3D12_RECT scissorRect{
			.left = static_cast<LONG>(cameraDesc.viewport.topLeftX),
			.top = static_cast<LONG>(cameraDesc.viewport.topLeftY),
			.right = static_cast<LONG>(cameraDesc.viewport.topLeftX + cameraDesc.viewport.width),
			.bottom = static_cast<LONG>(cameraDesc.viewport.topLeftY + cameraDesc.viewport.height),
		};
		commandList->RSSetScissorRects(1, &scissorRect);

		renderer_->Execute(commandList.Get(), cameraDesc.passType);

		const D3D12_RESOURCE_BARRIER toPresent{
			.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
			.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
			.Transition = {
				.pResource = backBuffer,
				.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
				.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET,
				.StateAfter = D3D12_RESOURCE_STATE_PRESENT,
			},
		};
		commandList->ResourceBarrier(1, &toPresent);
	}
}

std::expected<Renderer*, std::wstring> RenderSystem::CreateRenderer()
{
	auto renderer{ std::make_unique<Renderer>() };
	Renderer* result{ renderer.get() };
	renderers.push_back(std::move(renderer));
	return result;
}

std::expected<Renderer*, std::wstring> RenderSystem::CreateRenderer(const RendererOptions& options_)
{
	if (options_.window == nullptr)
	{
		return CreateRenderer();
	}

	return CreateRenderer();
}

void RenderSystem::DestroyRenderer(Renderer* renderer_)
{
	const auto it{ std::ranges::find_if(renderers, [renderer_](const std::unique_ptr<Renderer>& renderer)
		{
			return renderer.get() == renderer_;
		}) };

	if (it != renderers.end())
	{
		renderers.erase(it);
	}
}

std::expected<RenderContext*, std::wstring> RenderSystem::CreateRenderContext()
{
	auto context{ std::make_unique<RenderContext>() };
	RenderContext* result{ context.get() };
	renderContexts.push_back(std::move(context));
	return result;
}

void RenderSystem::DestroyRenderContext(RenderContext* context_)
{
	const auto it{ std::ranges::find_if(renderContexts, [context_](const std::unique_ptr<RenderContext>& context)
		{
			return context.get() == context_;
		}) };

	if (it != renderContexts.end())
	{
		renderContexts.erase(it);
	}
}

std::expected<void, std::wstring> RenderSystem::CreateRenderTarget(Window* window_, int width_, int height_)
{
	if (device == nullptr || factory == nullptr || commandQueue == nullptr)
	{
		return std::unexpected{ L"RenderSystem is not initialized." };
	}

	if (window_ == nullptr)
	{
		return std::unexpected{ L"Window is null." };
	}

	if (window_->GetHandle() == nullptr)
	{
		return std::unexpected{ L"Window handle is null." };
	}

	if (m_windowTargetPairs.size() >= MaxRenderTargets)
	{
		return std::unexpected{ L"Maximum render target count exceeded." };
	}

	if (FindWindowTargetPair(window_) != nullptr)
	{
		return {};
	}

	RenderTarget renderTarget{
		.windowHandle = window_->GetHandle(),
		.width = static_cast<uint32_t>(std::max(1, width_)),
		.height = static_cast<uint32_t>(std::max(1, height_)),
	};

	const DXGI_SWAP_CHAIN_DESC1 swapChainDesc{
		.Width = renderTarget.width,
		.Height = renderTarget.height,
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.Stereo = FALSE,
		.SampleDesc = { .Count = 1, .Quality = 0 },
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = RenderTarget::BufferCount,
		.Scaling = DXGI_SCALING_STRETCH,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
		.Flags = 0,
	};

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1{ nullptr };
	if (FAILED(factory->CreateSwapChainForHwnd(
		commandQueue.Get(),
		renderTarget.windowHandle,
		&swapChainDesc,
		nullptr,
		nullptr,
		swapChain1.GetAddressOf())))
	{
		return std::unexpected{ L"CreateSwapChainForHwnd failed." };
	}

	if (FAILED(swapChain1.As(&renderTarget.swapChain)))
	{
		return std::unexpected{ L"IDXGISwapChain3 query failed." };
	}

	renderTarget.currentBackBufferIndex = renderTarget.swapChain->GetCurrentBackBufferIndex();

	for (uint32_t i = 0; i < RenderTarget::BufferCount; ++i)
	{
		if (FAILED(renderTarget.swapChain->GetBuffer(i, IID_PPV_ARGS(renderTarget.backBuffers[i].GetAddressOf()))))
		{
			return std::unexpected{ L"SwapChain back buffer acquisition failed." };
		}

		const uint32_t descriptorIndex{ allocatedRtvCount++ };
		renderTarget.rtvHandles[i] = OffsetHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), descriptorIndex, rtvDescriptorSize);
		device->CreateRenderTargetView(renderTarget.backBuffers[i].Get(), nullptr, renderTarget.rtvHandles[i]);
	}

	const D3D12_HEAP_PROPERTIES heapProperties{
		.Type = D3D12_HEAP_TYPE_DEFAULT,
		.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
		.CreationNodeMask = 1,
		.VisibleNodeMask = 1,
	};

	const D3D12_RESOURCE_DESC depthDesc{
		.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		.Alignment = 0,
		.Width = renderTarget.width,
		.Height = renderTarget.height,
		.DepthOrArraySize = 1,
		.MipLevels = 1,
		.Format = DXGI_FORMAT_D32_FLOAT,
		.SampleDesc = { .Count = 1, .Quality = 0 },
		.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
		.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
	};

	const D3D12_CLEAR_VALUE depthClearValue{
		.Format = DXGI_FORMAT_D32_FLOAT,
		.DepthStencil = { .Depth = 1.0f, .Stencil = 0 },
	};

	if (FAILED(device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(renderTarget.depthStencilBuffer.GetAddressOf()))))
	{
		return std::unexpected{ L"Depth stencil buffer creation failed." };
	}

	renderTarget.dsvHandle = OffsetHandle(dsvHeap->GetCPUDescriptorHandleForHeapStart(), allocatedDsvCount++, dsvDescriptorSize);
	device->CreateDepthStencilView(renderTarget.depthStencilBuffer.Get(), nullptr, renderTarget.dsvHandle);

	m_windowTargetPairs.push_back(WindowTargetPair{
		.window = window_,
		.target = std::move(renderTarget),
	});
	activeWindow = window_;

	return {};
}

void RenderSystem::DestroyRenderTarget(Window* window_)
{
	const auto it{ std::ranges::find_if(m_windowTargetPairs, [window_](const WindowTargetPair& pair)
		{
			return pair.window == window_;
		}) };

	if (it == m_windowTargetPairs.end())
	{
		return;
	}

	if (activeWindow == window_)
	{
		activeWindow = nullptr;
	}

	m_windowTargetPairs.erase(it);
}

RenderSystem::WindowTargetPair* RenderSystem::FindWindowTargetPair(Window* window_) noexcept
{
	const auto it{ std::ranges::find_if(m_windowTargetPairs, [window_](const WindowTargetPair& pair)
		{
			return pair.window == window_;
		}) };

	if (it == m_windowTargetPairs.end())
	{
		return nullptr;
	}

	return &(*it);
}

const RenderSystem::WindowTargetPair* RenderSystem::FindWindowTargetPair(Window* window_) const noexcept
{
	const auto it{ std::ranges::find_if(m_windowTargetPairs, [window_](const WindowTargetPair& pair)
		{
			return pair.window == window_;
		}) };

	if (it == m_windowTargetPairs.end())
	{
		return nullptr;
	}

	return &(*it);
}

void RenderSystem::WaitForFrame(FrameResource& frameResource_)
{
	if (frameResource_.fenceValue == 0 || fence->GetCompletedValue() >= frameResource_.fenceValue)
	{
		return;
	}

	fence->SetEventOnCompletion(frameResource_.fenceValue, fenceEvent);
	::WaitForSingleObject(fenceEvent, INFINITE);
}

void RenderSystem::WaitForGpu()
{
	if (commandQueue == nullptr || fence == nullptr || fenceEvent == nullptr)
	{
		return;
	}

	const uint64_t signaledFenceValue{ nextFenceValue++ };
	commandQueue->Signal(fence.Get(), signaledFenceValue);

	if (fence->GetCompletedValue() < signaledFenceValue)
	{
		fence->SetEventOnCompletion(signaledFenceValue, fenceEvent);
		::WaitForSingleObject(fenceEvent, INFINITE);
	}
}

