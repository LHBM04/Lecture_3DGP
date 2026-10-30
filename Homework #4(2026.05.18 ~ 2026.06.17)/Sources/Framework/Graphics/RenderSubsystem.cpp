#include "Precompiled.hpp"
#include "RenderSubsystem.hpp"

#include "Framework/Platform/Display.hpp"


namespace
{
	/** DXGI Factory */
	Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory = nullptr;
}

namespace TUK::Framework
{
	RenderSubsystem::RenderSubsystem() noexcept
		: EngineSubsystem(100)
		, device(std::nullopt)
		, commandBuffer(std::nullopt)
		, swapChains()
		, fence(std::nullopt)
		, listeners()
	{
		swapChains.reserve(10);
	}

	RenderSubsystem::~RenderSubsystem() noexcept
	{
		dxgiFactory.Reset();
	}

	void RenderSubsystem::RegisterDisplay(const Display& display)
	{
		DXGI_SWAP_CHAIN_DESC1 description{};
		description.Width = display.GetClientSize().GetX();
		description.Height = display.GetClientSize().GetY();
		description.BufferCount = 2;
		description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		description.SampleDesc.Count = 1;

		auto newSwapChain = CreateD3DSwapChain(display.GetHWND(), description);
		if (!newSwapChain)
		{
			std::cerr << "Render Subsystem Display Registration 실패: " << newSwapChain.error() << std::endl;
			return;
		}

		swapChains.emplace(&display, std::move(newSwapChain.value()));
	}

	void RenderSubsystem::UnregisterDisplay(const Display& display)
	{
		auto result = swapChains.find(&display);
		if (result != swapChains.end())
		{
			swapChains.erase(result);
		}
	}

	void RenderSubsystem::Subscribe(std::function<void(D3DCommandBuffer&)> callback)
	{
		if (callback)
		{
			listeners.push_back(std::move(callback));
		}
	}

	const D3DDevice* RenderSubsystem::GetD3DDevice() const noexcept
	{
		return device ? &device.value() : nullptr;
	}

	const D3DSwapChain* RenderSubsystem::GetD3DSwapChain(const Display& display) const noexcept
	{
		const auto result = swapChains.find(&display);
		return result != swapChains.end() ? &result->second : nullptr;
	}

	std::expected<void, std::string> RenderSubsystem::WaitForGPU()
	{
		if (!device || !fence)
		{
			return std::unexpected("D3DDevice 또는 D3DFence가 초기화되지 않았습니다.");
		}
		if (auto result = fence->Signal(*device); !result)
		{
			return std::unexpected(result.error());
		}
		return fence->Wait();
	}

	SystemStatus RenderSubsystem::OnStartup()
	{
		// GPU 디바이스 생성
		{
			auto newDevice = CreateD3DDevice();
			if (!newDevice)
			{
				std::cerr << "Render Subsystem Startup 실패: " << newDevice.error() << std::endl;
				return SystemStatus::Failure;
			}

			device = std::move(newDevice.value());
		}

		// GPU 명령 버퍼 생성
		{
			auto newCommandBuffer = device->CreateD3DCommandBuffer();
			if (!newCommandBuffer)
			{
				std::cerr << "Render Subsystem Startup 실패: " << newCommandBuffer.error() << std::endl;
				return SystemStatus::Failure;
			}

			commandBuffer = std::move(newCommandBuffer.value());
		}

		// GPU 펜스 생성
		{
			auto newFence = device->CreateD3DFence();
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
		// 직전 프레임의 Fence 대기가 끝난 뒤 기록용 메모리를 재사용합니다.
		ID3D12CommandAllocator& allocator = commandBuffer->GetD3DCommandAllocator();
		ID3D12GraphicsCommandList10& commandList = commandBuffer->GetD3DCommandList();

		if (FAILED(allocator.Reset()))
		{
			std::cerr << "D3D12 Command Allocator Reset 실패!" << std::endl;
			return SystemStatus::Failure;
		}

		if (FAILED(commandList.Reset(&allocator, nullptr)))
		{
			std::cerr << "D3D12 Command List Reset 실패!" << std::endl;
			return SystemStatus::Failure;
		}

		// 각 스왑 체인을 돌면서 명령을 기록한다.
		for (auto& [display, swapChain] : swapChains)
		{
			const D3DTarget target = swapChain.GetCurrentBackBufferTarget();
			const D3DTexture& backBuffer = target.GetTexture();

			commandBuffer->TransitionTexture(backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			commandBuffer->ClearD3DTarget(target, ColorRGBA<float>::GetBlack());
			commandBuffer->SetD3DTarget(target);

			const D3D12_RESOURCE_DESC targetDescription = backBuffer.GetD3DResource().GetDesc();
			const D3D12_VIEWPORT viewport
			{
				0.0f, 0.0f,
				static_cast<float>(targetDescription.Width), static_cast<float>(targetDescription.Height),
				0.0f, 1.0f
			};
			const D3D12_RECT scissor
			{
				0, 0, static_cast<LONG>(targetDescription.Width), static_cast<LONG>(targetDescription.Height)
			};
			commandList.RSSetViewports(1, &viewport);
			commandList.RSSetScissorRects(1, &scissor);

			for (const RenderCallback& listener : listeners)
			{
				listener(*commandBuffer);
			}

			commandBuffer->TransitionTexture(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		}

		// 그리고 디바이스(정확히는 명령 큐)에 제출한다.
		if (auto result = device->SubmitCommandBuffer(*commandBuffer); !result)
		{
			std::cerr << result.error() << std::endl;
			return SystemStatus::Failure;
		}

		// 모든 스왑 체인을 Present합니다.
		for (auto& swapChain : swapChains | std::views::values)
		{
			swapChain.Present();
		}

		if (auto result = fence->Signal(*device); !result)
		{
			std::cerr << result.error() << std::endl;
			return SystemStatus::Failure;
		}
		
		if (auto result = fence->Wait(); !result)
		{
			std::cerr << result.error() << std::endl;
			return SystemStatus::Failure;
		}

		return SystemStatus::Continue;
	}

	void RenderSubsystem::OnShutdown()
	{
		if (device && fence)
		{
			auto signalResult = fence->Signal(*device);
			if (!signalResult)
			{
				std::cerr << signalResult.error() << std::endl;
			}
			else
			{
				auto waitResult = fence->Wait();
				if (!waitResult)
				{
					std::cerr << waitResult.error() << std::endl;
				}
			}
		}

		commandBuffer.reset();
		listeners.clear();
		swapChains.clear();
		fence.reset();
		device.reset();
		dxgiFactory.Reset();
	}

	std::expected<D3DDevice, std::string> RenderSubsystem::CreateD3DDevice() const
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

				return D3DDevice(std::move(d3dDevice), std::move(d3dCommandQueue));
			}
			
		}

		return std::unexpected("GPU 장치 생성 실패!");
	}

	std::expected<D3DSwapChain, std::string> RenderSubsystem::CreateD3DSwapChain(HWND window, const DXGI_SWAP_CHAIN_DESC1& description) const
	{
		if (!device)
		{
			return std::unexpected("D3DDevice가 초기화되지 않았습니다.");
		}

		if (!dxgiFactory)
		{
			return std::unexpected("DXGI Factory가 초기화되지 않았습니다.");
		}

		ID3D12Device& d3dDevice = device->GetD3DDevice();
		ID3D12CommandQueue& d3dCommandQueue = device->GetD3DCommandQueue();

		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
		if (FAILED(dxgiFactory->CreateSwapChainForHwnd(
			&d3dCommandQueue,
			window,
			&description,
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
		heapDesc.NumDescriptors = description.BufferCount;
		
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
		if (FAILED(d3dDevice.CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(rtvHeap.GetAddressOf()))))
		{
			return std::unexpected("RTV Descriptor Heap 생성 실패!");
		}

		const UINT descriptorSize = d3dDevice.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtv = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		
		std::vector<D3DTexture> backBuffers;
		backBuffers.reserve(description.BufferCount);

		for (std::uint32_t index = 0; index < description.BufferCount; ++index)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
			if (FAILED(swapChain4->GetBuffer(index, IID_PPV_ARGS(backBuffer.GetAddressOf()))))
			{
				return std::unexpected("D3D12 Back Buffer 가져오기 실패!");
			}

			backBuffers.push_back(D3DTexture(std::move(backBuffer)));
		}

		for (const D3DTexture& backBuffer : backBuffers)
		{
			d3dDevice.CreateRenderTargetView(&backBuffer.GetD3DResource(), nullptr, rtv);
			rtv.ptr += descriptorSize;
		}

		return D3DSwapChain(std::move(swapChain4), std::move(rtvHeap), std::move(backBuffers), descriptorSize);
	}
}
