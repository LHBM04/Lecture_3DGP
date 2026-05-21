#include "Precompiled.h"
#include "Renderer.h"

Renderer::~Renderer()
{
	Shutdown();
}

bool Renderer::Initialize(HWND windowHandle, UINT width, UINT height)
{
	try
	{
		EnableDebugLayer();

		UINT factoryFlags{ 0 };
		ThrowIfFailed(::CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory)), "DXGI factory creation failed.");

		if (!CreateDevice() ||
			!CreateCommandObjects() ||
			!CreateSwapChain(windowHandle, width, height) ||
			!CreateRenderTargets() ||
			!CreateFence())
		{
			Shutdown();
			return false;
		}

		initialized = true;
		return true;
	}
	catch (...)
	{
		Shutdown();
		return false;
	}
}

void Renderer::Render()
{
	if (!initialized)
	{
		return;
	}

	ID3D12CommandAllocator* commandAllocator{ frameResources[frameIndex].commandAllocator.Get() };
	ThrowIfFailed(commandAllocator->Reset(), "Command allocator reset failed.");
	ThrowIfFailed(commandList->Reset(commandAllocator, nullptr), "Command list reset failed.");

	RenderTarget& target{ renderTargets[frameIndex] };
	const FLOAT clearColor[]{ 0.08f, 0.1f, 0.14f, 1.0f };

	target.Transition(commandList.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
	target.Bind(commandList.Get());
	target.Clear(commandList.Get(), clearColor);
	target.Transition(commandList.Get(), D3D12_RESOURCE_STATE_PRESENT);

	ThrowIfFailed(commandList->Close(), "Command list close failed.");

	ID3D12CommandList* commandLists[]{ commandList.Get() };
	commandQueue->ExecuteCommandLists(1, commandLists);

	ThrowIfFailed(swapChain->Present(1, 0), "Swap chain present failed.");
	MoveToNextFrame();
}

void Renderer::Shutdown() noexcept
{
	if (commandQueue != nullptr && fence != nullptr)
	{
		WaitForGPU();
	}

	for (RenderTarget& target : renderTargets)
	{
		target.Reset();
	}

	if (fenceEvent != nullptr)
	{
		::CloseHandle(fenceEvent);
		fenceEvent = nullptr;
	}

	fence.Reset();
	commandList.Reset();
	for (FrameResource& frameResource : frameResources)
	{
		frameResource.commandAllocator.Reset();
		frameResource.fenceValue = 0;
	}
	rtvHeap.Reset();
	swapChain.Reset();
	commandQueue.Reset();
	device.Reset();
	factory.Reset();

	frameIndex = 0;
	rtvDescriptorSize = 0;
	initialized = false;
}

void Renderer::WaitForGPU()
{
	const UINT64 fenceValue{ frameResources[frameIndex].fenceValue };
	ThrowIfFailed(commandQueue->Signal(fence.Get(), fenceValue), "Fence signal failed.");
	ThrowIfFailed(fence->SetEventOnCompletion(fenceValue, fenceEvent), "Fence event registration failed.");
	::WaitForSingleObject(fenceEvent, INFINITE);
	++frameResources[frameIndex].fenceValue;
}

void Renderer::EnableDebugLayer()
{
#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(::D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif
}

bool Renderer::CreateDevice()
{
	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;

	for (UINT adapterIndex{ 0 };
		factory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND;
		++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 description{};
		adapter->GetDesc1(&description);

		if ((description.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0)
		{
			continue;
		}

		if (SUCCEEDED(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device))))
		{
			return true;
		}
	}

	return SUCCEEDED(::D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)));
}

bool Renderer::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)), "Command queue creation failed.");
	for (FrameResource& frameResource : frameResources)
	{
		ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frameResource.commandAllocator)), "Command allocator creation failed.");
	}

	ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frameResources[0].commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)), "Command list creation failed.");
	ThrowIfFailed(commandList->Close(), "Initial command list close failed.");

	return true;
}

bool Renderer::CreateSwapChain(HWND windowHandle, UINT width, UINT height)
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChainBase;
	ThrowIfFailed(factory->CreateSwapChainForHwnd(commandQueue.Get(), windowHandle, &swapChainDesc, nullptr, nullptr, &swapChainBase), "Swap chain creation failed.");
	ThrowIfFailed(factory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER), "Window association failed.");
	ThrowIfFailed(swapChainBase.As(&swapChain), "Swap chain cast failed.");

	frameIndex = swapChain->GetCurrentBackBufferIndex();
	return true;
}

bool Renderer::CreateRenderTargets()
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.NumDescriptors = FrameCount;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	ThrowIfFailed(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeap)), "RTV heap creation failed.");
	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };

	for (UINT index{ 0 }; index < FrameCount; ++index)
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
		ThrowIfFailed(swapChain->GetBuffer(index, IID_PPV_ARGS(&buffer)), "Swap chain buffer acquisition failed.");
		device->CreateRenderTargetView(buffer.Get(), nullptr, rtvHandle);
		renderTargets[index].Initialize(std::move(buffer), rtvHandle, D3D12_RESOURCE_STATE_PRESENT);
		rtvHandle.ptr += rtvDescriptorSize;
	}

	return true;
}

bool Renderer::CreateFence()
{
	ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "Fence creation failed.");

	for (FrameResource& frameResource : frameResources)
	{
		frameResource.fenceValue = 1;
	}
	fenceEvent = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);
	return fenceEvent != nullptr;
}

void Renderer::MoveToNextFrame()
{
	const UINT64 currentFenceValue{ frameResources[frameIndex].fenceValue };
	ThrowIfFailed(commandQueue->Signal(fence.Get(), currentFenceValue), "Frame fence signal failed.");

	frameIndex = swapChain->GetCurrentBackBufferIndex();

	if (fence->GetCompletedValue() < frameResources[frameIndex].fenceValue)
	{
		ThrowIfFailed(fence->SetEventOnCompletion(frameResources[frameIndex].fenceValue, fenceEvent), "Frame fence event registration failed.");
		::WaitForSingleObject(fenceEvent, INFINITE);
	}

	frameResources[frameIndex].fenceValue = currentFenceValue + 1;
}

void Renderer::ThrowIfFailed(HRESULT result, std::string_view message) const
{
	if (FAILED(result))
	{
		throw std::runtime_error{ message.data() };
	}
}
