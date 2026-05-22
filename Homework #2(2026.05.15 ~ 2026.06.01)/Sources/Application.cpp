#include "Precompiled.h"
#include "Application.h"

Engine::Engine(HINSTANCE instance_)
	: instance(instance_)
{
	WNDCLASSEXW wc;
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = instance;
	wc.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = L"D3D12 Framework";
	wc.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);

	RegisterClassExW(&wc) != 0;
}

Engine::~Engine()
{
	Shutdown();
}

bool Engine::Initialize(const Options& options_)
{
	options = options_;

	DWORD style = WS_OVERLAPPEDWINDOW;
	if (!options.resizable)
	{
		style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
	}

	if (options.borderless)
	{
		style = WS_POPUP;
	}

	RECT windowRect;
	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = options.width;
	windowRect.bottom = options.height;
	AdjustWindowRect(&windowRect, style, FALSE);

	window = CreateWindowExW(
		0,
		L"D3D12 Framework",
		options.title,
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		instance,
		this);

#if defined(_DEBUG)
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif

	CreateDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateRenderTargetViews();
	CreateDepthStencilView();
	CreateFence();

	return true;
}

int Engine::Run()
{	
	if (nullptr == window)
	{
		return -1;
	}

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);

	MSG message{};
	while (message.message != WM_QUIT)
	{
		if (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessageW(&message);
		}
		else
		{

		}
	}

	Shutdown();
	return static_cast<int>(message.wParam);
}

void Engine::Shutdown()
{
	WaitForGpu();

	if (fenceEvent != nullptr)
	{
		CloseHandle(fenceEvent);
		fenceEvent = nullptr;
	}

	for (Microsoft::WRL::ComPtr<ID3D12Resource>& backBuffer : backBuffers)
	{
		backBuffer.Reset();
	}

	depthStencilBuffer.Reset();
	dsvDescriptorHeap.Reset();
	rtvDescriptorHeap.Reset();
	swapChain.Reset();
	commandList.Reset();
	commandAllocator.Reset();
	commandQueue.Reset();
	fence.Reset();
	device.Reset();
	factory.Reset();

	if (window != nullptr)
	{
		DestroyWindow(window);
		window = nullptr;
	}

	UnregisterClassW(L"D3D12 Framework", instance);
}

bool Engine::CreateMainWindow()
{
	
	return window != nullptr;
}

bool Engine::CreateDevice()
{
	UINT factoryFlags = 0;
#if defined(_DEBUG)
	factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	if (FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory))))
	{
		if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))))
		{
			return false;
		}
	}

	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
	for (UINT adapterIndex = 0; factory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND; ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 desc{};
		adapter->GetDesc1(&desc);
		if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0)
		{
			continue;
		}

		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device))))
		{
			break;
		}
	}

	if (device == nullptr)
	{
		if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device))))
		{
			return false;
		}
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels{};
	qualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	qualityLevels.SampleCount = 4;
	qualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	qualityLevels.NumQualityLevels = 0;

	if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &qualityLevels, sizeof(qualityLevels))) &&
		qualityLevels.NumQualityLevels > 0)
	{
		msaa4xEnable = false;
		msaa4xQualityLevels = qualityLevels.NumQualityLevels;
	}

	return true;
}

bool Engine::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 0;

	if (FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue))))
	{
		return false;
	}

	if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator))))
	{
		return false;
	}

	if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList))))
	{
		return false;
	}

	return SUCCEEDED(commandList->Close());
}

bool Engine::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = static_cast<UINT>(options.width);
	swapChainDesc.Height = static_cast<UINT>(options.height);
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = BUFFER_COUNT;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = 0;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> tempSwapChain;
	if (FAILED(factory->CreateSwapChainForHwnd(commandQueue.Get(), window, &swapChainDesc, nullptr, nullptr, &tempSwapChain)))
	{
		return false;
	}

	factory->MakeWindowAssociation(window, DXGI_MWA_NO_ALT_ENTER);

	if (FAILED(tempSwapChain.As(&swapChain)))
	{
		return false;
	}

	currentBufferIndex = swapChain->GetCurrentBackBufferIndex();
	return true;
}

bool Engine::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.NumDescriptors = BUFFER_COUNT;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;

	if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap))))
	{
		return false;
	}

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;

	if (FAILED(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvDescriptorHeap))))
	{
		return false;
	}

	rtvDescriptorIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	dsvDescriptorIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	return true;
}

bool Engine::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT index = 0; index < BUFFER_COUNT; ++index)
	{
		if (FAILED(swapChain->GetBuffer(index, IID_PPV_ARGS(&backBuffers[index]))))
		{
			return false;
		}

		device->CreateRenderTargetView(backBuffers[index].Get(), nullptr, handle);
		handle.ptr += rtvDescriptorIncrementSize;
	}

	return true;
}

bool Engine::CreateDepthStencilView()
{
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 1;
	heapProperties.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = static_cast<UINT64>(options.width);
	resourceDesc.Height = static_cast<UINT>(options.height);
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	if (FAILED(device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(&depthStencilBuffer))))
	{
		return false;
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.Texture2D.MipSlice = 0;

	device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsvDesc, GetDepthStencilView());
	return true;
}

bool Engine::CreateFence()
{
	if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
	{
		return false;
	}

	fenceValues[currentBufferIndex] = 1;
	fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
	return fenceEvent != nullptr;
}

void Engine::Render()
{
	if (FAILED(commandAllocator->Reset()))
	{
		return;
	}

	if (FAILED(commandList->Reset(commandAllocator.Get(), nullptr)))
	{
		return;
	}

	D3D12_RESOURCE_BARRIER beginBarrier{};
	beginBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	beginBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	beginBarrier.Transition.pResource = backBuffers[currentBufferIndex].Get();
	beginBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	beginBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	beginBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList->ResourceBarrier(1, &beginBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtv = GetCurrentRenderTargetView();
	D3D12_CPU_DESCRIPTOR_HANDLE dsv = GetDepthStencilView();
	const float clearColor[]{ 0.12f, 0.15f, 0.22f, 1.0f };

	commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
	commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	D3D12_RESOURCE_BARRIER endBarrier{};
	endBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	endBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	endBarrier.Transition.pResource = backBuffers[currentBufferIndex].Get();
	endBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	endBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	endBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	commandList->ResourceBarrier(1, &endBarrier);

	if (FAILED(commandList->Close()))
	{
		return;
	}

	ID3D12CommandList* commandLists[]{ commandList.Get() };
	commandQueue->ExecuteCommandLists(1, commandLists);

	if (SUCCEEDED(swapChain->Present(1, 0)))
	{
		MoveToNextFrame();
	}
}

void Engine::Resize(int width_, int height_)
{
	if (width_ <= 0 || height_ <= 0)
	{
		return;
	}

	WaitForGpu();

	for (Microsoft::WRL::ComPtr<ID3D12Resource>& backBuffer : backBuffers)
	{
		backBuffer.Reset();
	}
	depthStencilBuffer.Reset();

	options.width = width_;
	options.height = height_;

	if (FAILED(swapChain->ResizeBuffers(BUFFER_COUNT, static_cast<UINT>(width_), static_cast<UINT>(height_), DXGI_FORMAT_R8G8B8A8_UNORM, 0)))
	{
		PostQuitMessage(-1);
		return;
	}

	currentBufferIndex = swapChain->GetCurrentBackBufferIndex();

	if (!CreateRenderTargetViews() || !CreateDepthStencilView())
	{
		PostQuitMessage(-1);
	}
}

void Engine::WaitForGpu()
{
	if (commandQueue == nullptr || fence == nullptr || fenceEvent == nullptr)
	{
		return;
	}

	const UINT64 fenceValue = fenceValues[currentBufferIndex];
	if (FAILED(commandQueue->Signal(fence.Get(), fenceValue)))
	{
		return;
	}

	if (fence->GetCompletedValue() < fenceValue)
	{
		if (SUCCEEDED(fence->SetEventOnCompletion(fenceValue, fenceEvent)))
		{
			WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
		}
	}

	++fenceValues[currentBufferIndex];
}

void Engine::MoveToNextFrame()
{
	const UINT64 currentFenceValue = fenceValues[currentBufferIndex];
	if (FAILED(commandQueue->Signal(fence.Get(), currentFenceValue)))
	{
		return;
	}

	currentBufferIndex = swapChain->GetCurrentBackBufferIndex();

	if (fence->GetCompletedValue() < fenceValues[currentBufferIndex])
	{
		if (SUCCEEDED(fence->SetEventOnCompletion(fenceValues[currentBufferIndex], fenceEvent)))
		{
			WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
		}
	}

	fenceValues[currentBufferIndex] = currentFenceValue + 1;
}

D3D12_CPU_DESCRIPTOR_HANDLE Engine::GetCurrentRenderTargetView() const
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += static_cast<SIZE_T>(currentBufferIndex) * rtvDescriptorIncrementSize;
	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE Engine::GetDepthStencilView() const
{
	return dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
}

LRESULT CALLBACK Engine::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCCREATE)
	{
		const CREATESTRUCTW* createStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Engine* application = reinterpret_cast<Engine*>(createStruct->lpCreateParams);
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(application));
	}
	else
	{
		Engine* application = reinterpret_cast<Engine*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

		switch (uMsg)
		{
			case WM_SIZE:
			{
				if (application != nullptr)
				{
					const int width = LOWORD(lParam);
					const int height = HIWORD(lParam);
					application->Resize(width, height);
				}
				return 0;
			}
			case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			}
			default:;
		}
	}

	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}
