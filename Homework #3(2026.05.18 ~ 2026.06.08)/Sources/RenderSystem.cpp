#include "Precompiled.h"
#include "RenderSystem.h"

#include "Logger.h"

#define SAFE_RELEASE(p) { if ((p)) { (p)->Release(); (p) = nullptr; } }

bool RenderSystem::Initialize(HWND window_)
{
	assert(window_ != nullptr);

	UINT factoryFlags{ 0 };
#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(::D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		debugController->Release();

		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	// Factory
	if (::CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory)) != S_OK)
	{
		Logger::Critical(L"Failed to create DXGI Factory.");
		return false;
	}

	// Device
	if (FAILED(CreateDevice()))
	{
		Logger::Critical(L"Failed to create D3D12 Device.");
		return false;
	}

	// Command Objects(Queue, List, Descriptor Heap)
	if (FAILED(CreateCommandObjects()))
	{
		Logger::Critical(L"Failed to create Command Objects.");
		return false;
	}

	// Swap Chain
	if (FAILED(CreateSwapChain(window_)))
	{
		Logger::Critical(L"Failed to create Swap Chain.");
		return false;
	}

	// Descriptor Heaps
	if (FAILED(CreateDescriptorHeaps()))
	{
		Logger::Critical(L"Failed to create Descriptor Heaps.");
		return false;
	}

	// Sync Objects(Fence, Event)
	if (FAILED(CreateSyncObjects()))
	{
		Logger::Critical(L"Failed to create Sync Objects.");
		return false;
	}

	// Render Target Views
	if (FAILED(CreateRenderTargetViews()))
	{
		Logger::Critical(L"Failed to create Render Target Views.");
		return false;
	}

	// Depth Stencil View
	if (FAILED(CreateDepthStencilView()))
	{
		Logger::Critical(L"Failed to create Depth Stencil View.");
		return false;
	}

	return true;
}

void RenderSystem::Release()
{

}

void RenderSystem::BeginFrame()
{

}

void RenderSystem::EndFrame()
{

}

void RenderSystem::Clear()
{
	SAFE_RELEASE(commandQueue);
}

void RenderSystem::Present()
{

}

ID3D12Device* RenderSystem::GetDevice() const noexcept
{
	return device.Get();
}

ID3D12GraphicsCommandList* RenderSystem::GetCommandList() const noexcept
{
	return commandList.Get();
}

HRESULT RenderSystem::CreateDevice()
{
	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
	for (UINT i{ 0 }; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(i, &adapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		adapter->GetDesc1(&dxgiAdapterDesc);

		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}
		
		if (SUCCEEDED(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device))))
		{
			break;
		}
	}

	if (adapter == nullptr)
	{
		factory->EnumWarpAdapter(_uuidof(IDXGIFactory4), (void**)&adapter);
		if (FAILED(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device))))
		{
			Logger::Critical(L"Failed to create WARP Device.");
		}
	}

	return S_OK;
}

HRESULT RenderSystem::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	if (FAILED(device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue))))
	{
		return E_FAIL;
	}

	if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators))))
	{
		return E_FAIL;
	}

	if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators.Get(), NULL, IID_PPV_ARGS(&commandList))))
	{
		return E_FAIL;
	}

	commandList->Close();

	return S_OK;
}

HRESULT RenderSystem::CreateSwapChain(HWND window_)
{
	RECT clientRect;
	::GetClientRect(window_, &clientRect);

	const int clientWidth{ static_cast<int>(clientRect.right - clientRect.left) };
	const int clientHeight{ static_cast<int>(clientRect.bottom - clientRect.top) };

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.BufferCount = BackBufferCount;
	swapChainDesc.Width = clientWidth;
	swapChainDesc.Height = clientHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = (msaa4xQualityLevels) ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = (isEnableMsaa4x) ? (msaa4xQualityLevels - 1) : 0;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
	factory->CreateSwapChainForHwnd(commandQueue.Get(), window_, &swapChainDesc, nullptr, nullptr, &swapChain1);
	swapChain1.As(&swapChain);
	frameIndex = swapChain->GetCurrentBackBufferIndex();

	return S_OK;
}

HRESULT RenderSystem::CreateDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.NumDescriptors = BackBufferCount;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptorHeapDesc.NodeMask = 0;
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
	device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	msaa4xQualityLevels = msaaQualityLevels.NumQualityLevels;
	isEnableMsaa4x = (msaa4xQualityLevels > 1) ? true : false;

	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	for (UINT i{ 0 }; i < BackBufferCount; i++)
	{
		fenceValues[i] = 0;
	}

	fenceEvent = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);

	return S_OK;
}

HRESULT RenderSystem::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	for (UINT count{ 0 }; count < BackBufferCount; count++)
	{
		if (FAILED(swapChain->GetBuffer(count, IID_PPV_ARGS(&backBuffers[count]))))
		{
			Logger::Critical(L"Failed to get buffer.");
			return E_FAIL;
		}
	
		device->CreateRenderTargetView(backBuffers[count].Get(), nullptr, rtvHandle);
		rtvHandle.ptr += rtvDescriptorSize;
	}

	return S_OK;
}

HRESULT RenderSystem::CreateDepthStencilView()
{
	RECT clientRect;
	::GetClientRect(::GetForegroundWindow(), &clientRect);
	const int clientWidth{ static_cast<int>(clientRect.right - clientRect.left) };
	const int clientHeight{ static_cast<int>(clientRect.bottom - clientRect.top) };

	D3D12_RESOURCE_DESC depthStencilDesc{};
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = clientWidth;
	depthStencilDesc.Height = clientHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = (msaa4xQualityLevels) ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = (isEnableMsaa4x) ? (msaa4xQualityLevels - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 1;
	heapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE depthOptimizedClearValue{};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	if (FAILED(device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&depthStencilBuffer))))
	{
		Logger::Critical(L"Failed to create Depth Buffer.");
		return E_FAIL;
	}

	device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, dsvHeap->GetCPUDescriptorHandleForHeapStart());
	
	return S_OK;
}
