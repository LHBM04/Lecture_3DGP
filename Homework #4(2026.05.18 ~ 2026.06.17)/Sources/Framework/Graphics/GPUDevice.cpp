#include "Precompiled.hpp"
#include "GPUDevice.hpp"

namespace TUK::Framework
{
	std::expected<GPUBuffer, std::string> GPUDevice::CreateGPUBuffer(const GPUBufferDescription& info) const
	{
		if (!d3dDevice)
		{
			return std::unexpected("GPUDevice가 초기화되지 않았습니다.");
		}

		if (info.sizeInBytes == 0 || info.sizeInBytes > std::numeric_limits<std::size_t>::max())
		{
			return std::unexpected("GPUBuffer 크기가 유효하지 않습니다.");
		}

		D3D12_RESOURCE_STATES initialState = info.initialState;
		switch (info.heapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT:
			break;
		case D3D12_HEAP_TYPE_UPLOAD:
			initialState = D3D12_RESOURCE_STATE_GENERIC_READ;
			break;
		case D3D12_HEAP_TYPE_READBACK:
			initialState = D3D12_RESOURCE_STATE_COPY_DEST;
			break;
		default:
			return std::unexpected("지원하지 않는 GPUBuffer Heap Type입니다.");
		}

		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = info.heapType;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC description{};
		description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		description.Width = info.sizeInBytes;
		description.Height = 1;
		description.DepthOrArraySize = 1;
		description.MipLevels = 1;
		description.SampleDesc.Count = 1;
		description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

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

		return GPUBuffer(std::move(resource), info.heapType);
	}

	std::expected<GPUMesh, std::string> GPUDevice::CreateGPUMesh(
		GPUBuffer vertexBuffer,
		const GPUMeshCreateInfo& info,
		std::optional<GPUBuffer> indexBuffer) const
	{
		if (!vertexBuffer.d3dResource || vertexBuffer.GetD3DHeapType() == D3D12_HEAP_TYPE_READBACK)
		{
			return std::unexpected("정점 버퍼가 유효하지 않습니다.");
		}

		const std::uint64_t vertexSize = vertexBuffer.GetSizeInBytes();
		if (info.vertexStride == 0 || info.vertexStride > D3D12_REQ_MULTI_ELEMENT_STRUCTURE_SIZE_IN_BYTES
			|| vertexSize > std::numeric_limits<std::uint32_t>::max()
			|| vertexSize % info.vertexStride != 0)
		{
			return std::unexpected("정점 버퍼 크기 또는 stride가 유효하지 않습니다.");
		}

		switch (info.topology)
		{
		case PrimitiveTopology::PointList:
		case PrimitiveTopology::LineList:
		case PrimitiveTopology::LineStrip:
		case PrimitiveTopology::TriangleList:
		case PrimitiveTopology::TriangleStrip:
			break;
		default:
			return std::unexpected("지원하지 않는 메쉬 topology입니다.");
		}

		if (indexBuffer)
		{
			if (!indexBuffer->d3dResource || indexBuffer->GetD3DHeapType() == D3D12_HEAP_TYPE_READBACK)
			{
				return std::unexpected("인덱스 버퍼가 유효하지 않습니다.");
			}

			if (info.indexFormat != DXGI_FORMAT_R16_UINT && info.indexFormat != DXGI_FORMAT_R32_UINT)
			{
				return std::unexpected("인덱스 형식은 R16_UINT 또는 R32_UINT여야 합니다.");
			}

			const std::uint32_t indexSize = info.indexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4;
			const std::uint64_t bufferSize = indexBuffer->GetSizeInBytes();
			if (bufferSize > std::numeric_limits<std::uint32_t>::max() || bufferSize % indexSize != 0)
			{
				return std::unexpected("인덱스 버퍼 크기가 유효하지 않습니다.");
			}
		}

		return GPUMesh(std::move(vertexBuffer), std::move(indexBuffer), info);
	}

	std::expected<GPUShader, std::string> GPUDevice::CreateGPUShader(const GPUShaderCreateInfo& info) const
	{
		if (info.path.empty() || info.entryPoint.empty() || info.target.empty())
		{
			return std::unexpected("GPUShader 파일 경로, 진입점과 타깃을 지정해야 합니다.");
		}

		Microsoft::WRL::ComPtr<ID3DBlob> d3dBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
		const HRESULT result = D3DCompileFromFile(
			info.path.c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			info.entryPoint.c_str(),
			info.target.c_str(),
			info.compileFlags,
			0,
			d3dBlob.GetAddressOf(),
			errorBlob.GetAddressOf());

		if (FAILED(result))
		{
			std::string message = std::format(
				"GPUShader 컴파일에 실패했습니다. Entry: {}, Target: {}, HRESULT: 0x{:08X}",
				info.entryPoint, info.target, static_cast<std::uint32_t>(result));

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

	std::expected<GPURootSignature, std::string> GPUDevice::CreateGPURootSignature(const GPURootSignatureCreateInfo& info) const
	{
		if (!d3dDevice)
		{
			return std::unexpected("GPUDevice가 초기화되지 않았습니다.");
		}

		if (info.parameters.size() > std::numeric_limits<std::uint32_t>::max()
			|| info.staticSamplers.size() > std::numeric_limits<std::uint32_t>::max())
		{
			return std::unexpected("GPURootSignature 파라미터 또는 정적 샘플러 개수가 유효하지 않습니다.");
		}

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC description{};
		description.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
		description.Desc_1_0.NumParameters = static_cast<std::uint32_t>(info.parameters.size());
		description.Desc_1_0.pParameters = info.parameters.data();
		description.Desc_1_0.NumStaticSamplers = static_cast<std::uint32_t>(info.staticSamplers.size());
		description.Desc_1_0.pStaticSamplers = info.staticSamplers.data();
		description.Desc_1_0.Flags = info.flags;

		Microsoft::WRL::ComPtr<ID3DBlob> serializedBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
		const HRESULT serializeResult = D3D12SerializeVersionedRootSignature(
			&description, serializedBlob.GetAddressOf(), errorBlob.GetAddressOf());
		if (FAILED(serializeResult))
		{
			std::string message = std::format(
				"GPURootSignature 직렬화에 실패했습니다. HRESULT: 0x{:08X}",
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
				"GPURootSignature 생성에 실패했습니다. HRESULT: 0x{:08X}",
				static_cast<std::uint32_t>(createResult)));
		}

		return GPURootSignature(std::move(d3dRootSignature));
	}

	std::expected<GPUGraphicsPipelineState, std::string> GPUDevice::CreateGPUGraphicsPipelineState(const GPUGraphicsPipelineStateCreateInfo& info) const
	{
		if (!d3dDevice)
		{
			return std::unexpected("GPUDevice가 초기화되지 않았습니다.");
		}

		if (!info.rootSignature.d3dRootSignature || !info.vertexShader.d3dBlob
			|| (info.pixelShader && !info.pixelShader->d3dBlob))
		{
			return std::unexpected("GPUGraphicsPipelineState의 루트 시그니처 또는 셰이더가 유효하지 않습니다.");
		}

		if (info.inputElements.size() > D3D12_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT
			|| info.renderTargetFormats.size() > D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT
			|| info.sampleDescription.Count == 0)
		{
			return std::unexpected("입력 요소 개수, 렌더 타겟 개수 또는 샘플 개수가 유효하지 않습니다.");
		}

		switch (info.topologyType)
		{
		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT:
		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE:
		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE:
			break;
		default:
			return std::unexpected("현재 그래픽스 파이프라인은 POINT, LINE, TRIANGLE topology만 지원합니다.");
		}

		if ((info.depthStencilState.DepthEnable || info.depthStencilState.StencilEnable)
			&& info.depthStencilFormat == DXGI_FORMAT_UNKNOWN)
		{
			return std::unexpected("깊이 또는 스텐실을 활성화하려면 depthStencilFormat을 지정해야 합니다.");
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC description{};
		description.pRootSignature = &info.rootSignature.GetD3DRootSignature();
		description.VS = info.vertexShader.GetD3DBytecode();
		if (info.pixelShader)
		{
			description.PS = info.pixelShader->GetD3DBytecode();
		}
		description.BlendState = info.blendState;
		description.SampleMask = info.sampleMask;
		description.RasterizerState = info.rasterizerState;
		description.DepthStencilState = info.depthStencilState;
		description.InputLayout = { info.inputElements.data(), static_cast<std::uint32_t>(info.inputElements.size()) };
		description.IBStripCutValue = info.stripCutValue;
		description.PrimitiveTopologyType = info.topologyType;
		description.NumRenderTargets = static_cast<std::uint32_t>(info.renderTargetFormats.size());
		for (std::size_t index = 0; index < info.renderTargetFormats.size(); ++index)
		{
			description.RTVFormats[index] = info.renderTargetFormats[index];
		}
		description.DSVFormat = info.depthStencilFormat;
		description.SampleDesc = info.sampleDescription;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> d3dPipelineState;
		const HRESULT result = d3dDevice->CreateGraphicsPipelineState(
			&description, IID_PPV_ARGS(d3dPipelineState.GetAddressOf()));
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

		return GPUCommandBuffer(std::move(d3dCommandList), std::move(d3dCommandAllocator));
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

	GPUDevice::GPUDevice(
		Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice,
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3dCommandQueue) noexcept
		: d3dDevice(std::move(d3dDevice))
		, d3dCommandQueue(std::move(d3dCommandQueue))
	{
	}
}
