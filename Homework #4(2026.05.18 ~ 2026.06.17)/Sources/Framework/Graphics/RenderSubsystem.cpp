#include "Precompiled.hpp"
#include "RenderSubsystem.hpp"

#include "Framework/Platform/Display.hpp"

namespace
{
	/** DXGI Factory */
	IDXGIFactory6* dxgiFactory = nullptr;
}

namespace TUK::Framework
{
	RenderSubsystem::RenderSubsystem() noexcept
		: EngineSubsystem(100)
		, device(std::nullopt)
		, commandBuffer(std::nullopt)
		, fence(std::nullopt)
		, swapChains()
	{
		swapChains.reserve(10);
	}

	RenderSubsystem::~RenderSubsystem() noexcept
	{
		if (dxgiFactory)
		{
			dxgiFactory->Release();
			dxgiFactory = nullptr;
		}
	}

	void RenderSubsystem::RegisterDisplay(const Display& display)
	{
		GPUSwapChainProperties properties{};
		properties.display = &display;
		properties.width = display.GetClientSize().GetX();
		properties.height = display.GetClientSize().GetY();
		properties.bufferCount = 2;

		auto newSwapChain = CreateGPUSwapChain(properties);
		if (!newSwapChain)
		{
			std::cerr << "Render Subsystem Display Registration 실패: " << newSwapChain.error() << std::endl;
			return;
		}

		swapChains.emplace(&display, std::move(newSwapChain.value()));
	}

	void RenderSubsystem::UnregisterDisplay(const Display& display)
	{
		// TODO: 이거 구현해라.
	}

	SystemStatus RenderSubsystem::OnStartup()
	{
		// GPU 디바이스 초기화
		{
			auto newDevice = CreateGPUDevice();
			if (!newDevice)
			{
				std::cerr << "Render Subsystem Startup 실패: " << newDevice.error() << std::endl;
				return SystemStatus::Failure;
			}

			device = std::move(newDevice.value());
		}

		// GPU 명령 버퍼 초기화
		{
			auto newCommandBuffer = device->CreateGPUCommandBuffer();
			if (!newCommandBuffer)
			{
				std::cerr << "Render Subsystem Startup 실패: " << newCommandBuffer.error() << std::endl;
				return SystemStatus::Failure;
			}

			commandBuffer = std::move(newCommandBuffer.value());
		}

		// GPU 펜스 초기화
		{
			auto newFence = device->CreateGPUFence();
			if (!newFence)
			{
				std::cerr << "Render Subsystem Startup 실패: " << newFence.error() << std::endl;
				return SystemStatus::Failure;
			}

			fence = std::move(newFence.value());
		}

		return SystemStatus::Continue;
	}

	SystemStatus RenderSubsystem::OnPreTick()
	{
		return SystemStatus::Continue;
	}

	SystemStatus RenderSubsystem::OnPostTick()
	{
		for (auto& swapChain : swapChains | std::views::values)
		{
			commandBuffer->BeginRenderPass(swapChain);
			commandBuffer->EndRenderPass();
		}

		device->SubmitCommandBuffer(*commandBuffer);

		for (auto& swapChain : swapChains | std::views::values)
		{
			swapChain.Present();
		}

		return SystemStatus::Continue;
	}

	void RenderSubsystem::OnShutdown()
	{
		if (dxgiFactory)
		{
			dxgiFactory->Release();
			dxgiFactory = nullptr;
		}
	}

	std::expected<GPUDevice, std::string> RenderSubsystem::CreateGPUDevice()
	{
		UINT factoryFlags = 0;

#ifdef _DEBUG
		ID3D12Debug* debugController = nullptr;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			debugController->Release();
		}
#endif

		if (FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&dxgiFactory))))
		{
			return std::unexpected("DXGI Factory 생성 실패!");
		}

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3dCommandQueue;

		for (UINT adapterIndex = 0; ; ++adapterIndex)
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
			if (FAILED(dxgiFactory->EnumAdapters1(adapterIndex, adapter.GetAddressOf())))
			{
				break;
			}

			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;
			}

			Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice;
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(d3dDevice.GetAddressOf()))))
			{
				D3D12_COMMAND_QUEUE_DESC queueDesc{};
				queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
				queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
				queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
				queueDesc.NodeMask = 0;

				if (FAILED(d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(d3dCommandQueue.GetAddressOf()))))
				{
					return std::unexpected("D3D12 Command Queue 생성 실패!");
				}

				return GPUDevice(std::move(d3dDevice), std::move(d3dCommandQueue));
			}
			
		}

		return std::unexpected("GPU 장치 생성 실패!");
	}

	std::expected<GPUSwapChain, std::string> RenderSubsystem::CreateGPUSwapChain(const GPUSwapChainProperties& properties)
	{
		if (!device)
		{
			return std::unexpected("GPUDevice가 초기화되지 않았습니다.");
		}

		ID3D12Device& d3dDevice = device->GetD3DDevice();
		ID3D12CommandQueue& d3dCommandQueue = device->GetD3DCommandQueue();

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
		swapChainDesc.BufferCount = properties.bufferCount;
		swapChainDesc.Width = properties.width;
		swapChainDesc.Height = properties.height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
		if (FAILED(dxgiFactory->CreateSwapChainForHwnd(
			&d3dCommandQueue,
			properties.display->GetHWND(),
			&swapChainDesc,
			nullptr,
			nullptr,
			swapChain1.GetAddressOf())))
		{
			return std::unexpected("DXGI Swap Chain 생성 실패!");
		}

		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain4;
		if (FAILED(swapChain1.As(&swapChain4)))
		{
			return std::unexpected("IDXGISwapChain4 인터페이스 조회 실패!");
		}

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDesc.NumDescriptors = properties.bufferCount;
		
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
		if (FAILED(d3dDevice.CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(rtvHeap.GetAddressOf()))))
		{
			return std::unexpected("RTV Descriptor Heap 생성 실패!");
		}

		const UINT descriptorSize = d3dDevice.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtv = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		
		std::vector<GPUTarget> renderTargets;
		renderTargets.reserve(properties.bufferCount);
		
		for (UINT index = 0; index < properties.bufferCount; ++index)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource2> backBuffer;
			if (FAILED(swapChain4->GetBuffer(index, IID_PPV_ARGS(backBuffer.GetAddressOf()))))
			{
				return std::unexpected("Swap Chain 백 버퍼 조회 실패!");
			}
		
			d3dDevice.CreateRenderTargetView(backBuffer.Get(), nullptr, rtv);
			renderTargets.emplace_back(std::move(backBuffer), rtv);
			rtv.ptr += descriptorSize;
		}

		return GPUSwapChain(std::move(swapChain4), std::move(rtvHeap), std::move(renderTargets));
	}
}
