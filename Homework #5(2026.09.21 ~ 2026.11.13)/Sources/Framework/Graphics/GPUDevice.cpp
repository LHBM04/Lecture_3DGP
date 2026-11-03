#include "Precompiled.hpp"
#include "GPUDevice.hpp"

#include <array>
#include <format>

namespace TUK::Framework
{
	std::expected<GPUDevice, std::string> GPUDevice::Create()
	{
		UINT factoryFlags = 0;

#ifdef _DEBUG
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
		{
			debugController->EnableDebugLayer();
			factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif

		ComPtr<IDXGIFactory6> dxgiFactory;
		if (FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(dxgiFactory.GetAddressOf()))))
		{
			return std::unexpected("DXGI Factory 생성 실패!");
		}

		for (UINT adapterIndex = 0; ; ++adapterIndex)
		{
			ComPtr<IDXGIAdapter1> adapter;
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

			ComPtr<ID3D12Device> d3dDevice;
			if (FAILED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(d3dDevice.GetAddressOf()))))
			{
				continue;
			}

			D3D12_COMMAND_QUEUE_DESC queueDescription{};
			queueDescription.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			queueDescription.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			queueDescription.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			queueDescription.NodeMask = 0;

			ComPtr<ID3D12CommandQueue> d3dCommandQueue;
			if (FAILED(d3dDevice->CreateCommandQueue(&queueDescription, IID_PPV_ARGS(d3dCommandQueue.GetAddressOf()))))
			{
				return std::unexpected("D3D12 Command Queue 생성 실패!");
			}

			constexpr u32 CONSTANT_BUFFER_SLOT_COUNT = 3;
			std::array<CD3DX12_ROOT_PARAMETER1, CONSTANT_BUFFER_SLOT_COUNT> parameters{};
			for (u32 index = 0; index < CONSTANT_BUFFER_SLOT_COUNT; ++index)
			{
				parameters[index].InitAsConstantBufferView(
					index,
					0,
					D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE);
			}

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription{};
			rootSignatureDescription.Init_1_1(
				CONSTANT_BUFFER_SLOT_COUNT,
				parameters.data(),
				0,
				nullptr,
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			ComPtr<ID3DBlob> serializedBlob;
			ComPtr<ID3DBlob> errorBlob;
			const HRESULT serializeResult = D3DX12SerializeVersionedRootSignature(
				&rootSignatureDescription,
				D3D_ROOT_SIGNATURE_VERSION_1_1,
				serializedBlob.GetAddressOf(),
				errorBlob.GetAddressOf());
			if (FAILED(serializeResult))
			{
				std::string message = std::format(
					"공통 루트 시그니처 직렬화에 실패했습니다. HRESULT: 0x{:08X}",
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

			ComPtr<ID3D12RootSignature> d3dRootSignature;
			const HRESULT createResult = d3dDevice->CreateRootSignature(
				0,
				serializedBlob->GetBufferPointer(),
				serializedBlob->GetBufferSize(),
				IID_PPV_ARGS(d3dRootSignature.GetAddressOf()));
			if (FAILED(createResult))
			{
				return std::unexpected(std::format(
					"공통 루트 시그니처 생성에 실패했습니다. HRESULT: 0x{:08X}",
					static_cast<u32>(createResult)));
			}

			return GPUDevice(
				std::move(dxgiFactory),
				std::move(d3dDevice),
				std::move(d3dCommandQueue),
				std::move(d3dRootSignature));
		}

		return std::unexpected("GPU 장치 생성 실패!");
	}
}
