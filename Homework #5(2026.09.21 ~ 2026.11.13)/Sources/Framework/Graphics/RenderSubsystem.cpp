#include "Precompiled.hpp"
#include "RenderSubsystem.hpp"

#include <array>
#include <format>
#include <iostream>
#include <ranges>
#include <utility>

#include "../Platform/Display.hpp"
#include "../Platform/DisplayEvents.hpp"

#include "../Core/Common.hpp"

#include "GPUSampler.hpp"
#include "GPUTexture2D.hpp"

namespace TUK::Framework
{
	SystemStatus RenderSubsystem::OnStartup()
	{
		auto newDevice = CreateDevice();
		if (!newDevice)
		{
			LOG_CRITICAL("GPU 디바이스 생성에 실패했습니다: {}", newDevice.error());
			return SystemStatus::FAILURE;
		}
		device = std::move(*newDevice);

		auto newCommandBuffer = device->CreateCommandBuffer();
		if (!newCommandBuffer)
		{
			LOG_ERROR("GPU 명령 버퍼 생성에 실패했습니다: {}", newCommandBuffer.error());
			return SystemStatus::FAILURE;
		}
		commandBuffer = std::move(*newCommandBuffer);

		auto newFence = device->CreateFence();
		if (!newFence)
		{
			LOG_ERROR("GPU 펜스 생성에 실패했습니다: {}", newFence.error());
			return SystemStatus::FAILURE;
		}
		fence = std::move(*newFence);

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
		if (auto result = commandBuffer->Reset(); !result)
		{
			LOG_ERROR("{}", result.error());
			return SystemStatus::FAILURE;
		}

		for (auto& [display, swapChain] : swapChains)
		{
			const GPUTarget target = swapChain.GetCurrentBackBufferTarget();
			const GPUTexture2D& backBuffer = target.GetTexture();

			commandBuffer->TransitionTexture(backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			commandBuffer->SetGPUTarget(target);

			for (const RenderCallback& listener : listeners)
			{
				listener(*commandBuffer);
			}

			commandBuffer->TransitionTexture(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		}

		if (auto result = commandBuffer->Submit(); !result)
		{
			LOG_CRITICAL("GPU 명령 버퍼 제출에 실패했습니다: {}", result.error());
			return SystemStatus::FAILURE;
		}

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
		listeners.clear();
		swapChains.clear();
		commandBuffer.reset();
		fence.reset();
		device.reset();
	}

	void RenderSubsystem::OnDisplayCreate(const DisplayCreateEvent& event)
	{
		const Display* display = event.display;
		if (!display)
		{
			return;
		}

		DXGI_SWAP_CHAIN_DESC1 description{};
		description.Width = display->GetClientSize().GetX();
		description.Height = display->GetClientSize().GetY();
		description.BufferCount = 2;
		description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		description.SampleDesc.Count = 1;

		auto result = device->CreateSwapChain(display, description);
		if (!result)
		{
			LOG_ERROR("GPU 스왑체인 생성에 실패했습니다: {}", result.error());
			return;
		}

		swapChains.emplace(display->GetID(), std::move(*result));
	}

	void RenderSubsystem::OnDisplayClose(const DisplayCloseEvent& event)
	{
		const Display* display = event.display;
		if (!display)
		{
			return;
		}

		auto result = swapChains.find(display->GetID());
		if (result != swapChains.end())
		{
			swapChains.erase(result);
		}
	}

	std::expected<GPUDevice, std::string> RenderSubsystem::CreateDevice() const
	{
		UINT factoryFlags = 0;

	#ifdef _DEBUG
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
		{
			debugController->EnableDebugLayer();
			factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	#endif

		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;
		if (FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(dxgiFactory.GetAddressOf()))))
		{
			return std::unexpected("DXGI 팩토리 생성에 실패했습니다.");
		}

		for (UINT adapterIndex = 0; ; ++adapterIndex)
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
			if (FAILED(dxgiFactory->EnumAdapters1(adapterIndex, adapter.GetAddressOf())))
			{
				break;
			}

			DXGI_ADAPTER_DESC1 adapterDescription{};
			adapter->GetDesc1(&adapterDescription);
			if (adapterDescription.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;
			}

			Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice;
			if (FAILED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(d3dDevice.GetAddressOf()))))
			{
				continue;
			}

			D3D12_COMMAND_QUEUE_DESC queueDescription{};
			queueDescription.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			queueDescription.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			queueDescription.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			queueDescription.NodeMask = 0;

			Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3dCommandQueue;
			if (FAILED(d3dDevice->CreateCommandQueue(&queueDescription, IID_PPV_ARGS(d3dCommandQueue.GetAddressOf()))))
			{
				return std::unexpected("D3D12 명령 큐 생성에 실패했습니다.");
			}

			constexpr u32 CONSTANT_BUFFER_SLOT_COUNT = 3;
			std::array<CD3DX12_ROOT_PARAMETER1, CONSTANT_BUFFER_SLOT_COUNT + 1> parameters{};
			for (u32 index = 0; index < CONSTANT_BUFFER_SLOT_COUNT; ++index)
			{
				parameters[index].InitAsConstantBufferView(
					index,
					0,
					D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE);
			}

			CD3DX12_DESCRIPTOR_RANGE1 textureRange{};
			textureRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, GPUCommandBuffer::TEXTURE_SLOT_COUNT, 0);
			parameters.back().InitAsDescriptorTable(1, &textureRange, D3D12_SHADER_VISIBILITY_PIXEL);

			const std::array samplers =
			{
				GPUSampler::LinearWrap().ToStaticDescription(0),
				GPUSampler::LinearClamp().ToStaticDescription(1),
				GPUSampler::PointClamp().ToStaticDescription(2)
			};

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription{};
			rootSignatureDescription.Init_1_1(
				static_cast<u32>(parameters.size()),
				parameters.data(),
				static_cast<u32>(samplers.size()),
				samplers.data(),
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			Microsoft::WRL::ComPtr<ID3DBlob> serializedBlob;
			Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
			const HRESULT serializeResult = D3DX12SerializeVersionedRootSignature(
				&rootSignatureDescription,
				D3D_ROOT_SIGNATURE_VERSION_1_1,
				serializedBlob.GetAddressOf(),
				errorBlob.GetAddressOf());
			if (FAILED(serializeResult))
			{
				std::string message = std::format(
					"공유 루트 시그니처 직렬화에 실패했습니다. HRESULT: 0x{:08X}",
					static_cast<u32>(serializeResult));
				if (errorBlob && errorBlob->GetBufferSize() > 0)
				{
					const char* text = static_cast<const char*>(errorBlob->GetBufferPointer());
					usize length = errorBlob->GetBufferSize();
					if (text[length - 1] == '\0')
					{
						--length;
					}

					message += '\n';
					message.append(text, length);
				}

				return std::unexpected(std::move(message));
			}

			Microsoft::WRL::ComPtr<ID3D12RootSignature> d3dRootSignature;
			const HRESULT createResult = d3dDevice->CreateRootSignature(
				0,
				serializedBlob->GetBufferPointer(),
				serializedBlob->GetBufferSize(),
				IID_PPV_ARGS(d3dRootSignature.GetAddressOf()));
			if (FAILED(createResult))
			{
				return std::unexpected(std::format(
					"공유 루트 시그니처 생성에 실패했습니다. HRESULT: 0x{:08X}",
					static_cast<u32>(createResult)));
			}

			return GPUDevice(GPUDeviceCreateInfo{
				.dxgiFactory = std::move(dxgiFactory),
				.d3dDevice = std::move(d3dDevice),
				.d3dCommandQueue = std::move(d3dCommandQueue),
				.d3dRootSignature = std::move(d3dRootSignature)
			});
		}

		return std::unexpected("GPU 디바이스 생성에 실패했습니다.");
	}

	void RenderSubsystem::AddListener(RenderCallback callback)
	{
		listeners.push_back(std::move(callback));
	}
}
