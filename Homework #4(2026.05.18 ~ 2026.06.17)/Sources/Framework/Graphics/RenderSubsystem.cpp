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
		}

		swapChains.emplace(&display, std::move(newSwapChain.value()));
	}

	void RenderSubsystem::UnregisterDisplay()
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

		return SystemStatus::Continue;
	}

	SystemStatus RenderSubsystem::OnPreTick()
	{
		return SystemStatus::Continue;
	}

	SystemStatus RenderSubsystem::OnPostTick()
	{
		for (auto& [display, swapChain] : swapChains)
		{
			D3D12_RENDER_PASS_RENDER_TARGET_DESC renderTargetDesc{};

			D3D12_RENDER_PASS_DEPTH_STENCIL_DESC depthStencilDesc{};

			commandBuffer.
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

		ID3D12CommandQueue* d3dCommandQueue = nullptr;
		if (FAILED(dxgiFactory->EnumAdapters1(0, nullptr)))
		{
			return std::unexpected("DXGI 어댑터 열거 실패!");
		}

		for (UINT adapterIndex = 0; ; ++adapterIndex)
		{
			IDXGIAdapter1* adapter = nullptr;
			if (FAILED(dxgiFactory->EnumAdapters1(adapterIndex, &adapter)))
			{
				break; // 더 이상 어댑터가 없음
			}

			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				adapter->Release();
				continue; // 소프트웨어 어댑터는 건너뜀
			}

			ID3D12Device* d3dDevice = nullptr;
			if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&d3dDevice))))
			{
				D3D12_COMMAND_QUEUE_DESC queueDesc = {};
				queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // 명령 큐의 타입 (DIRECT, COMPUTE, COPY 등)
				queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // 우선순위
				queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; // 플래그 (예: 타임아웃 방지 등)
				queueDesc.NodeMask = 0; // 단일 GPU인 경우 0

				HRESULT hr = d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&d3dCommandQueue));
				if (FAILED(hr))
				{
					return std::unexpected("D3D12 Command Queue 생성 실패!");
				}

				adapter->Release();
				return GPUDevice(d3dDevice, d3dCommandQueue);
			}
			
			adapter->Release();
		}

		return std::unexpected("GPU 장치 생성 실패!");
	}

	std::expected<GPUSwapChain, std::string> RenderSubsystem::CreateGPUSwapChain(const GPUSwapChainProperties& properties)
	{
		if (!device)
		{
			return std::unexpected("GPUDevice가 초기화되지 않았습니다.");
		}

		ID3D12Device& d3dDevice = device.value().GetD3DDevice();

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = properties.bufferCount;
		swapChainDesc.Width = properties.width;
		swapChainDesc.Height = properties.height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = {};
		fullscreenDesc.Windowed = TRUE;

		IDXGISwapChain1* d3dSwapChain1 = nullptr;
		if (FAILED(dxgiFactory->CreateSwapChainForHwnd(
			&d3dDevice,
			properties.display.GetHWND(),
			&swapChainDesc,
			&fullscreenDesc,
			nullptr,
			&d3dSwapChain1
		)))
		{
			return std::unexpected("DXGI Swap Chain 생성 실패!");
		}

		IDXGISwapChain4* d3dSwapChain4 = nullptr;
		if (FAILED(d3dSwapChain1->QueryInterface(IID_PPV_ARGS(&d3dSwapChain4))))
		{
			d3dSwapChain1->Release();
			return std::unexpected("DXGI Swap Chain 4 -> 1 변환 실패!");
		}

		return GPUSwapChain(d3dSwapChain4);
	}
}
