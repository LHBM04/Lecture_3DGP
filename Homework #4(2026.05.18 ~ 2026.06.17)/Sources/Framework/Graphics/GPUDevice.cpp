#include "Precompiled.hpp"
#include "GPUDevice.hpp"

#include <array>

namespace TUK::Framework
{
	std::expected<GPUBuffer, std::string> GPUDevice::CreateGPUBuffer(const D3D12_HEAP_PROPERTIES& heapProperties, const D3D12_RESOURCE_DESC& description, D3D12_RESOURCE_STATES initialState) const
	{
		if (!d3dDevice)
		{
			return std::unexpected("GPUDevice가 초기화되지 않았습니다.");
		}

		if (description.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER || description.Width == 0
			|| description.Width > std::numeric_limits<std::size_t>::max())
		{
			return std::unexpected("GPUBuffer 크기 또는 리소스 형식이 유효하지 않습니다.");
		}

		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		if (FAILED(d3dDevice->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&description,
			initialState,
			nullptr,
			IID_PPV_ARGS(resource.GetAddressOf()))))
		{
			return std::unexpected("GPUBuffer 리소스 생성에 실패했습니다.");
		}

		return GPUBuffer(std::move(resource), heapProperties.Type);
	}

	std::expected<GPUMesh, std::string> GPUDevice::CreateGPUMesh(
		GPUBuffer vertexBuffer,
		std::uint32_t vertextStride,
		GPUBuffer indexBuffer,
		DXGI_FORMAT indexFormat,
		D3D12_PRIMITIVE_TOPOLOGY topology) const
	{
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
		vertexBufferView.BufferLocation = vertexBuffer.GetGPUVirtualAddress();
		vertexBufferView.SizeInBytes = static_cast<UINT>(vertexBuffer.GetSizeInBytes());
		vertexBufferView.StrideInBytes = vertextStride;

		D3D12_INDEX_BUFFER_VIEW indexBufferView{};
		indexBufferView.BufferLocation = indexBuffer.GetGPUVirtualAddress();
		indexBufferView.SizeInBytes = static_cast<UINT>(indexBuffer.GetSizeInBytes());
		indexBufferView.Format = indexFormat;

		return GPUMesh(std::move(vertexBuffer), vertexBufferView, std::move(indexBuffer), indexBufferView, topology);
	}

	std::expected<GPUShader, std::string> GPUDevice::CreateGPUShader(const std::filesystem::path& path, std::string_view target, const std::string& entryPoint, std::uint32_t compileFlags) const
	{
		Microsoft::WRL::ComPtr<ID3DBlob> d3dBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

		const HRESULT result = D3DCompileFromFile(
			path.c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPoint.c_str(),
			target.data(),
			compileFlags,
			0,
			d3dBlob.GetAddressOf(),
			errorBlob.GetAddressOf());

		if (FAILED(result))
		{
			std::string message = std::format(
				"GPUShader 컴파일에 실패했습니다. Entry: {}, Target: {}, HRESULT: 0x{:08X}",
				entryPoint, target.data(), static_cast<std::uint32_t>(result));

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

		return GPUShader(std::move(d3dBlob));
	}

	std::expected<void, std::string> GPUDevice::InitializeRootSignature()
	{
		if (!d3dDevice)
		{
			return std::unexpected("GPUDevice가 초기화되지 않았습니다.");
		}

		// b0: 카메라, b1: 오브젝트, b2: 머터리얼 (모두 space0).
		constexpr std::uint32_t CONSTANT_BUFFER_SLOT_COUNT = 3;
		std::array<D3D12_ROOT_PARAMETER, CONSTANT_BUFFER_SLOT_COUNT> parameters{};
		for (std::uint32_t index = 0; index < CONSTANT_BUFFER_SLOT_COUNT; ++index)
		{
			parameters[index].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			parameters[index].Descriptor.ShaderRegister = index;
			parameters[index].Descriptor.RegisterSpace = 0;
			parameters[index].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		}

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC description{};
		description.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
		description.Desc_1_0.NumParameters = CONSTANT_BUFFER_SLOT_COUNT;
		description.Desc_1_0.pParameters = parameters.data();
		description.Desc_1_0.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		Microsoft::WRL::ComPtr<ID3DBlob> serializedBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
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

		return {};
	}

	std::expected<GPUGraphicsPipelineState, std::string> GPUDevice::CreateGPUGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& description) const
	{
		if (!d3dDevice)
		{
			return std::unexpected("GPUDevice가 초기화되지 않았습니다.");
		}

		if (!d3dRootSignature)
		{
			return std::unexpected("공통 루트 시그니처가 초기화되지 않았습니다.");
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDescription = description;
		pipelineDescription.pRootSignature = d3dRootSignature.Get();

		Microsoft::WRL::ComPtr<ID3D12PipelineState> d3dPipelineState;
		const HRESULT result = d3dDevice->CreateGraphicsPipelineState(
			&pipelineDescription, IID_PPV_ARGS(d3dPipelineState.GetAddressOf()));
		if (FAILED(result))
		{
			return std::unexpected(std::format(
				"GPUGraphicsPipelineState 생성에 실패했습니다. HRESULT: 0x{:08X}",
				static_cast<std::uint32_t>(result)));
		}

		return GPUGraphicsPipelineState(std::move(d3dPipelineState));
	}

	std::expected<GPUCommandBuffer, std::string> GPUDevice::CreateGPUCommandBuffer() const
	{
		if (!d3dDevice)
		{
			return std::unexpected("GPUDevice가 초기화되지 않았습니다.");
		}

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> d3dCommandAllocator;
		if (FAILED(d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(d3dCommandAllocator.GetAddressOf()))))
		{
			return std::unexpected("D3D12 Command Allocator 생성에 실패했습니다.");
		}

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> d3dCommandList;
		if (FAILED(d3dDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			d3dCommandAllocator.Get(),
			nullptr,
			IID_PPV_ARGS(d3dCommandList.GetAddressOf()))))
		{
			return std::unexpected("D3D12 Command List 생성에 실패했습니다.");
		}

		if (FAILED(d3dCommandList->Close()))
		{
			return std::unexpected("D3D12 Command List 초기 Close에 실패했습니다.");
		}

		return GPUCommandBuffer(*this, std::move(d3dCommandList), std::move(d3dCommandAllocator));
	}

	std::expected<GPUFence, std::string> GPUDevice::CreateGPUFence() const
	{
		if (!d3dDevice)
		{
			return std::unexpected("GPUDevice가 초기화되지 않았습니다.");
		}
		
		Microsoft::WRL::ComPtr<ID3D12Fence> d3dFence;
		if (FAILED(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(d3dFence.GetAddressOf()))))
		{
			return std::unexpected("D3D12 Fence 생성에 실패했습니다.");
		}

		return GPUFence(std::move(d3dFence));
	}
	
	std::expected<void, std::string> GPUDevice::SubmitCommandBuffer(GPUCommandBuffer& commandBuffer)
	{
		ID3D12GraphicsCommandList10& d3dCommandList = commandBuffer.GetD3DCommandList();
		if (FAILED(d3dCommandList.Close()))
		{
			return std::unexpected("D3D12 Command List Close에 실패했습니다.");
		}

		ID3D12CommandList* commandLists[] = { &d3dCommandList };
		d3dCommandQueue->ExecuteCommandLists(1, commandLists);

		return {};
	}

	ID3D12Device& GPUDevice::GetD3DDevice() const noexcept
	{
		return *(d3dDevice.Get());
	}

	ID3D12CommandQueue& GPUDevice::GetD3DCommandQueue() const noexcept
	{
		return *(d3dCommandQueue.Get());
	}

	ID3D12RootSignature& GPUDevice::GetD3DRootSignature() const noexcept
	{
		assert(d3dRootSignature);
		return *d3dRootSignature.Get();
	}

	GPUDevice::GPUDevice(
		Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice,
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3dCommandQueue) noexcept
		: d3dDevice(std::move(d3dDevice))
		, d3dCommandQueue(std::move(d3dCommandQueue))
	{
	}
}
