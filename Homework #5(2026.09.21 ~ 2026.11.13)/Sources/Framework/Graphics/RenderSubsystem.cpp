#include "Precompiled.hpp"
#include "RenderSubsystem.hpp"

#include "../Platform/Display.hpp"
#include "../Platform/DisplayEvents.hpp"

#include "GPUTexture.hpp"

namespace TUK::Framework
{
	SystemStatus RenderSubsystem::OnStartup()
	{
		// GPU 디바이스 생성
		{
			auto newDevice = CreateGPUDevice();
			if (!newDevice)
			{
				std::cerr << "Render Subsystem Startup 실패: " << newDevice.error() << std::endl;
				return SystemStatus::FAILURE;
			}

			device = std::move(newDevice.value());
		}

		// GPU 명령 버퍼 생성
		{
			auto newCommandBuffer = device->CreateGPUCommandBuffer();
			if (!newCommandBuffer)
			{
				std::cerr << "Render Subsystem Startup 실패: " << newCommandBuffer.error() << std::endl;
				return SystemStatus::FAILURE;
			}

			commandBuffer = std::move(newCommandBuffer.value());
		}

		// GPU 펜스 생성
		{
			auto newFence = device->CreateGPUFence();
			if (!newFence)
			{
				std::cerr << "Render Subsystem Startup 실패: " << newFence.error() << std::endl;
				return SystemStatus::FAILURE;
			}

			fence = std::move(newFence.value());
		}

		return SystemStatus::CONTINUE;
	}

	SystemStatus RenderSubsystem::OnPreTick()
	{
		return SystemStatus::CONTINUE;
	}

	SystemStatus RenderSubsystem::OnTick()
	{
		return SystemStatus::CONTINUE;
	}

	SystemStatus RenderSubsystem::OnPostTick()
	{
		commandBuffer->Reset();

		// 각 스왑 체인을 돌면서 명령을 기록.
		for (auto& [display, swapChain] : swapChains)
		{
			const GPUTarget target = swapChain.GetCurrentBackBufferTarget();
			const GPUTexture& backBuffer = target.GetTexture();

			for (const RenderCallback& listener : listeners)
			{
				listener(*commandBuffer);
			}

			commandBuffer->TransitionTexture(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		}

		commandBuffer->Submit();

		// 모든 스왑 체인을 Present.
		for (auto& swapChain : swapChains | std::views::values)
		{
			swapChain.Present();
		}

		fence->Signal(*device);
		fence->Wait();

		return SystemStatus::CONTINUE;
	}

	void RenderSubsystem::OnShutdown()
	{
	}

	void RenderSubsystem::OnDisplayCreated(const DisplayCreateEvent& event)
	{
		const Display& display = event.display;

		DXGI_SWAP_CHAIN_DESC1 description{};
		description.Width = display.GetClientSize().GetX();
		description.Height = display.GetClientSize().GetY();
		description.BufferCount = 2;
		description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		description.SampleDesc.Count = 1;

		auto result = CreateGPUSwapChain(display, description);
		if (!result)
		{
			std::cerr << "Render Subsystem Display Registration 실패: " << result.error() << std::endl;
			return;
		}

		swapChains.emplace(display.GetID(), std::move(result.value()));
	}

	void RenderSubsystem::OnDisplayClosed(const DisplayCloseEvent& event)
	{
		const Display& display = event.display;

		auto result = swapChains.find(display.GetID());
		if (result != swapChains.end())
		{
			swapChains.erase(result);
		}
	}

	void RenderSubsystem::AddListener(RenderCallback callback)
	{
		listeners.push_back(std::move(callback));
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

		ComPtr<ID3D12CommandQueue> d3dCommandQueue;

		for (UINT adapterIndex = 0; ; ++adapterIndex)
		{
			ComPtr<IDXGIAdapter1> adapter;
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

			ComPtr<ID3D12Device> d3dDevice;
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

				ComPtr<ID3D12RootSignature> d3dRootSignature;

				constexpr std::uint32_t CONSTANT_BUFFER_SLOT_COUNT = 3;
				std::array<D3D12_ROOT_PARAMETER, CONSTANT_BUFFER_SLOT_COUNT> parameters{};
				for (std::uint32_t index = 0; index < CONSTANT_BUFFER_SLOT_COUNT; ++index)
				{
					CD3DX12_ROOT_PARAMETER::InitAsConstantBufferView(parameters[index], index);
				}

				const CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC description(
					CONSTANT_BUFFER_SLOT_COUNT, parameters.data(), 0, nullptr,
					D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

				ComPtr<ID3DBlob> serializedBlob;
				ComPtr<ID3DBlob> errorBlob;
				const HRESULT serializeResult = D3D12SerializeVersionedRootSignature(
					&description, serializedBlob.GetAddressOf(), errorBlob.GetAddressOf());
				if (FAILED(serializeResult))
				{
					std::string message = std::format(
						"공통 루트 시그니처 직렬화에 실패했습니다. HRESULT: 0x{:08X}",
						static_cast<std::uint32_t>(serializeResult));
					if (errorBlob && errorBlob->GetBufferSize() > 0)
					{
						const char* text = static_cast<const char*>(errorBlob->GetBufferPointer());
						std::size_t length = errorBlob->GetBufferSize();
						if (text[length - 1] == '\0')
						{
							--length;
						}

						message += '\n';
						message.append(text, length);
					}
					return std::unexpected(std::move(message));
				}

				const HRESULT createResult = d3dDevice->CreateRootSignature(
					0,
					serializedBlob->GetBufferPointer(),
					serializedBlob->GetBufferSize(),
					IID_PPV_ARGS(d3dRootSignature.GetAddressOf()));
				if (FAILED(createResult))
				{
					return std::unexpected(std::format(
						"공통 루트 시그니처 생성에 실패했습니다. HRESULT: 0x{:08X}",
						static_cast<std::uint32_t>(createResult)));
				}

				GPUDeviceProperties properties
				{
					.d3dDevice = d3dDevice,
					.d3dCommandQueue = d3dCommandQueue,
					.d3dRootSignature = d3dRootSignature
				};

				return GPUDevice(properties);
			}

		}

		return std::unexpected("GPU 장치 생성 실패!");
	}

	std::expected<GPUSwapChain, std::string> RenderSubsystem::CreateGPUSwapChain(
		const Display& display,
		const DXGI_SWAP_CHAIN_DESC1& description)
	{
		if (!device)
		{
			return std::unexpected("GPUDevice가 초기화되지 않았습니다.");
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
			display.GetHWND(),
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
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(rtvHeap->GetCPUDescriptorHandleForHeapStart());

		std::pmr::vector<GPUTexture> backBuffers;
		backBuffers.reserve(description.BufferCount);

		for (std::uint32_t index = 0; index < description.BufferCount; ++index)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
			if (FAILED(swapChain4->GetBuffer(index, IID_PPV_ARGS(backBuffer.GetAddressOf()))))
			{
				return std::unexpected("D3D12 Back Buffer 가져오기 실패!");
			}

			backBuffers.push_back(GPUTexture(std::move(backBuffer)));
		}

		for (const GPUTexture& backBuffer : backBuffers)
		{
			d3dDevice.CreateRenderTargetView(&backBuffer.GetD3DResource(), nullptr, rtv);
			rtv.Offset(1, descriptorSize);
		}

		GPUSwapChainProperties properties{};
		properties.dxgiSwapChain = std::move(swapChain4);
		properties.rtvDescriptorSize = descriptorSize;
		properties.d3dDescriptorHeap = std::move(rtvHeap);
		properties.backBuffers = std::move(backBuffers);

		return GPUSwapChain(std::move(properties));
	}
}
