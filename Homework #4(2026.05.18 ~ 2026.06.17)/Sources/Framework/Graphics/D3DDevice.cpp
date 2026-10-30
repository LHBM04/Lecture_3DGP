#include "Precompiled.hpp"
#include "D3DDevice.hpp"

namespace TUK::Framework
{
	std::expected<D3DBuffer, std::string> D3DDevice::CreateD3DBuffer(const D3D12_HEAP_PROPERTIES& heapProperties, const D3D12_RESOURCE_DESC& description, D3D12_RESOURCE_STATES initialState) const
	{
		if (!d3dDevice)
		{
			return std::unexpected("D3DDevice가 초기화되지 않았습니다.");
		}

		if (description.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER || description.Width == 0
			|| description.Width > std::numeric_limits<std::size_t>::max())
		{
			return std::unexpected("D3DBuffer 크기 또는 리소스 형식이 유효하지 않습니다.");
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
			return std::unexpected("D3DBuffer 리소스 생성에 실패했습니다.");
		}

		return D3DBuffer(std::move(resource), heapProperties.Type);
	}

	std::expected<D3DMesh, std::string> D3DDevice::CreateD3DMesh(
		D3DBuffer vertexBuffer,
		D3DBuffer indexBuffer,
		std::uint32_t vertexStride, DXGI_FORMAT indexFormat, D3D12_PRIMITIVE_TOPOLOGY topology) const
	{
		if (!vertexBuffer.d3dResource || vertexBuffer.GetD3DHeapType() == D3D12_HEAP_TYPE_READBACK)
		{
			return std::unexpected("정점 버퍼가 유효하지 않습니다.");
		}

		const std::uint64_t vertexSize = vertexBuffer.GetSizeInBytes();
		if (vertexStride == 0 || vertexStride > D3D12_REQ_MULTI_ELEMENT_STRUCTURE_SIZE_IN_BYTES
			|| vertexSize > std::numeric_limits<std::uint32_t>::max()
			|| vertexSize % vertexStride != 0)
		{
			return std::unexpected("정점 버퍼 크기 또는 stride가 유효하지 않습니다.");
		}

		switch (topology)
		{
		case D3D_PRIMITIVE_TOPOLOGY_POINTLIST:
		case D3D_PRIMITIVE_TOPOLOGY_LINELIST:
		case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP:
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
			break;
		default:
			return std::unexpected("지원하지 않는 메쉬 topology입니다.");
		}

		if (!indexBuffer.d3dResource || indexBuffer.GetD3DHeapType() == D3D12_HEAP_TYPE_READBACK)
		{
			return std::unexpected("인덱스 버퍼가 유효하지 않습니다.");
		}

		if (indexFormat != DXGI_FORMAT_R16_UINT && indexFormat != DXGI_FORMAT_R32_UINT)
		{
			return std::unexpected("인덱스 형식은 R16_UINT 또는 R32_UINT여야 합니다.");
		}

		const std::uint32_t indexSize = indexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4;
		const std::uint64_t bufferSize = indexBuffer.GetSizeInBytes();
		if (bufferSize > std::numeric_limits<std::uint32_t>::max() || bufferSize % indexSize != 0)
		{
			return std::unexpected("인덱스 버퍼 크기가 유효하지 않습니다.");
		}

		return D3DMesh(std::move(vertexBuffer), std::move(indexBuffer), vertexStride, indexFormat, topology);
	}

	std::expected<D3DShader, std::string> D3DDevice::CreateD3DShader(const std::filesystem::path& path, std::string_view target, const std::string& entryPoint, std::uint32_t compileFlags) const
	{
		if (path.empty() || entryPoint.empty() || target.empty() || target.find('\0') != std::string_view::npos)
		{
			return std::unexpected("D3DShader 파일 경로, 진입점과 유효한 대상 프로파일을 지정해야 합니다.");
		}

		std::string targetString(target);

		Microsoft::WRL::ComPtr<ID3DBlob> d3dBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
		const HRESULT result = D3DCompileFromFile(
			path.c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPoint.c_str(),
			targetString.c_str(),
			compileFlags,
			0,
			d3dBlob.GetAddressOf(),
			errorBlob.GetAddressOf());

		if (FAILED(result))
		{
			std::string message = std::format(
				"D3DShader 컴파일에 실패했습니다. Entry: {}, Target: {}, HRESULT: 0x{:08X}",
				entryPoint, targetString, static_cast<std::uint32_t>(result));

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

		return D3DShader(std::move(d3dBlob), std::move(targetString));
	}

	std::expected<D3DRootSignature, std::string> D3DDevice::CreateD3DRootSignature(const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& description) const
	{
		if (!d3dDevice)
		{
			return std::unexpected("D3DDevice가 초기화되지 않았습니다.");
		}

		Microsoft::WRL::ComPtr<ID3DBlob> serializedBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
		const HRESULT serializeResult = D3D12SerializeVersionedRootSignature(
			&description, serializedBlob.GetAddressOf(), errorBlob.GetAddressOf());
		if (FAILED(serializeResult))
		{
			std::string message = std::format(
				"D3DRootSignature 직렬화에 실패했습니다. HRESULT: 0x{:08X}",
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

		Microsoft::WRL::ComPtr<ID3D12RootSignature> d3dRootSignature;
		const HRESULT createResult = d3dDevice->CreateRootSignature(
			0,
			serializedBlob->GetBufferPointer(),
			serializedBlob->GetBufferSize(),
			IID_PPV_ARGS(d3dRootSignature.GetAddressOf()));
		if (FAILED(createResult))
		{
			return std::unexpected(std::format(
				"D3DRootSignature 생성에 실패했습니다. HRESULT: 0x{:08X}",
				static_cast<std::uint32_t>(createResult)));
		}

		return D3DRootSignature(std::move(d3dRootSignature));
	}

	std::expected<D3DGraphicsPipelineState, std::string> D3DDevice::CreateD3DGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& description) const
	{
		if (!d3dDevice)
		{
			return std::unexpected("D3DDevice가 초기화되지 않았습니다.");
		}

		Microsoft::WRL::ComPtr<ID3D12PipelineState> d3dPipelineState;
		const HRESULT result = d3dDevice->CreateGraphicsPipelineState(
			&description, IID_PPV_ARGS(d3dPipelineState.GetAddressOf()));
		if (FAILED(result))
		{
			return std::unexpected(std::format(
				"D3DGraphicsPipelineState 생성에 실패했습니다. HRESULT: 0x{:08X}",
				static_cast<std::uint32_t>(result)));
		}

		return D3DGraphicsPipelineState(std::move(d3dPipelineState));
	}

	std::expected<D3DCommandBuffer, std::string> D3DDevice::CreateD3DCommandBuffer() const
	{
		if (!d3dDevice)
		{
			return std::unexpected("D3DDevice가 초기화되지 않았습니다.");
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

		return D3DCommandBuffer(std::move(d3dCommandList), std::move(d3dCommandAllocator));
	}

	std::expected<D3DFence, std::string> D3DDevice::CreateD3DFence() const
	{
		if (!d3dDevice)
		{
			return std::unexpected("D3DDevice가 초기화되지 않았습니다.");
		}
		
		Microsoft::WRL::ComPtr<ID3D12Fence> d3dFence;
		if (FAILED(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(d3dFence.GetAddressOf()))))
		{
			return std::unexpected("D3D12 Fence 생성에 실패했습니다.");
		}

		return D3DFence(std::move(d3dFence));
	}
	
	std::expected<void, std::string> D3DDevice::SubmitCommandBuffer(D3DCommandBuffer& commandBuffer)
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

	ID3D12Device& D3DDevice::GetD3DDevice() const noexcept
	{
		return *(d3dDevice.Get());
	}

	ID3D12CommandQueue& D3DDevice::GetD3DCommandQueue() const noexcept
	{
		return *(d3dCommandQueue.Get());
	}

	D3DDevice::D3DDevice(
		Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice,
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3dCommandQueue) noexcept
		: d3dDevice(std::move(d3dDevice))
		, d3dCommandQueue(std::move(d3dCommandQueue))
	{
	}
}
