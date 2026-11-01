#include "Precompiled.hpp"
#include "GPUDevice.hpp"

namespace TUK::Framework
{
	std::expected<GPUBuffer, std::string> GPUDevice::CreateGPUBuffer(const D3D12_HEAP_PROPERTIES& heapProperties, const D3D12_RESOURCE_DESC& description, D3D12_RESOURCE_STATES initialState) const
	{
		if (!d3dDevice)
		{
			return std::unexpected("GPUDevice가 초기화되지 않았습니다.");
		}

		if (description.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER || description.Width == 0
			|| description.Width > std::numeric_limits<usize>::max())
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

		return GPUBuffer(GPUBufferProperties{ std::move(resource), heapProperties.Type });
	}

	std::expected<GPUMesh, std::string> GPUDevice::CreateGPUMesh(
		GPUBuffer vertexBuffer,
		u32 vertexStride,
		GPUBuffer indexBuffer,
		DXGI_FORMAT indexFormat,
		D3D12_PRIMITIVE_TOPOLOGY topology) const
	{
		if (vertexStride == 0 || vertexBuffer.GetSizeInBytes() == 0
			|| vertexBuffer.GetSizeInBytes() > std::numeric_limits<u32>::max()
			|| vertexBuffer.GetSizeInBytes() % vertexStride != 0)
		{
			return std::unexpected("정점 버퍼 크기 또는 스트라이드가 유효하지 않습니다.");
		}
		if (indexFormat != DXGI_FORMAT_R16_UINT && indexFormat != DXGI_FORMAT_R32_UINT)
		{
			return std::unexpected("인덱스 형식은 R16_UINT 또는 R32_UINT여야 합니다.");
		}
		const u32 INDEX_SIZE = indexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4;
		if (indexBuffer.GetSizeInBytes() == 0
			|| indexBuffer.GetSizeInBytes() > std::numeric_limits<u32>::max()
			|| indexBuffer.GetSizeInBytes() % INDEX_SIZE != 0)
		{
			return std::unexpected("인덱스 버퍼 크기가 유효하지 않습니다.");
		}

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
		vertexBufferView.BufferLocation = vertexBuffer.GetGPUVirtualAddress();
		vertexBufferView.SizeInBytes = static_cast<UINT>(vertexBuffer.GetSizeInBytes());
		vertexBufferView.StrideInBytes = vertexStride;

		D3D12_INDEX_BUFFER_VIEW indexBufferView{};
		indexBufferView.BufferLocation = indexBuffer.GetGPUVirtualAddress();
		indexBufferView.SizeInBytes = static_cast<UINT>(indexBuffer.GetSizeInBytes());
		indexBufferView.Format = indexFormat;

		return GPUMesh(GPUMeshProperties{ std::move(vertexBuffer), vertexBufferView, std::move(indexBuffer), indexBufferView, topology });
	}

	std::expected<GPUShader, std::string> GPUDevice::CreateGPUShader(
		const std::filesystem::path& path, std::wstring_view target, std::wstring_view entryPoint) const
	{
		ComPtr<IDxcUtils> utils;
		ComPtr<IDxcCompiler3> compiler;
		ComPtr<IDxcIncludeHandler> includeHandler;
		if (FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf())))
			|| FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(compiler.GetAddressOf())))
			|| FAILED(utils->CreateDefaultIncludeHandler(includeHandler.GetAddressOf())))
		{
			return std::unexpected("DXC 초기화에 실패했습니다.");
		}

		ComPtr<IDxcBlobEncoding> source;
		HRESULT result = utils->LoadFile(path.c_str(), nullptr, source.GetAddressOf());
		if (FAILED(result))
		{
			return std::unexpected(std::format("셰이더 파일 읽기 실패: {}, HRESULT: 0x{:08X}", path.string(), static_cast<u32>(result)));
		}

		const std::wstring TARGET(target);
		const std::wstring ENTRY_POINT(entryPoint);
		const std::wstring INCLUDE_DIRECTORY = path.has_parent_path() ? path.parent_path().wstring() : L".";
		const wchar_t* arguments[] =
		{
			path.c_str(), L"-E", ENTRY_POINT.c_str(), L"-T", TARGET.c_str(),
			L"-I", INCLUDE_DIRECTORY.c_str(), L"-Ges", L"-WX", L"-HV", L"2018"
		};
		BOOL knownEncoding = FALSE;
		UINT32 codePage = DXC_CP_UTF8;
		source->GetEncoding(&knownEncoding, &codePage);
		const DxcBuffer SOURCE_BUFFER{ source->GetBufferPointer(), source->GetBufferSize(), knownEncoding ? codePage : DXC_CP_UTF8 };
		ComPtr<IDxcResult> compilation;
		result = compiler->Compile(&SOURCE_BUFFER, arguments, static_cast<UINT32>(std::size(arguments)),
			includeHandler.Get(), IID_PPV_ARGS(compilation.GetAddressOf()));
		if (FAILED(result))
		{
			return std::unexpected(std::format("DXC 호출 실패: 0x{:08X}", static_cast<u32>(result)));
		}

		HRESULT status = E_FAIL;
		result = compilation->GetStatus(&status);
		if (FAILED(result) || FAILED(status))
		{
			std::string message = std::format("GPUShader 컴파일 실패: {}, HRESULT: 0x{:08X}",
				path.string(), static_cast<u32>(FAILED(result) ? result : status));
			ComPtr<IDxcBlobUtf8> errors;
			if (SUCCEEDED(compilation->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(errors.GetAddressOf()), nullptr))
				&& errors && errors->GetStringLength() > 0)
			{
				message += '\n';
				message.append(errors->GetStringPointer(), errors->GetStringLength());
			}
			return std::unexpected(std::move(message));
		}

		ComPtr<IDxcBlob> object;
		result = compilation->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(object.GetAddressOf()), nullptr);
		if (FAILED(result) || !object || object->GetBufferSize() == 0)
		{
			return std::unexpected("DXC 셰이더 바이트코드를 가져오지 못했습니다.");
		}
		return GPUShader(std::move(object));
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

		return GPUCommandBuffer(*this, GPUCommandBufferProperties{ std::move(d3dCommandList), std::move(d3dCommandAllocator) });
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

		ComPtr<ID3D12PipelineState> d3dPipelineState;
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

	std::expected<GPUFence, std::string> GPUDevice::CreateGPUFence() const
	{
		if (!d3dDevice)
		{
			return std::unexpected("GPUDevice가 초기화되지 않았습니다.");
		}

		ComPtr<ID3D12Fence> d3dFence;
		if (FAILED(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(d3dFence.GetAddressOf()))))
		{
			return std::unexpected("D3D12 Fence 생성에 실패했습니다.");
		}

		return GPUFence(std::move(d3dFence));
	}
}
