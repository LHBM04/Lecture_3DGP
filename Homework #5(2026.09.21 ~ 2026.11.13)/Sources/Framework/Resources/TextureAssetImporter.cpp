#include "Precompiled.hpp"
#include "TextureAssetImporter.hpp"

#include <cctype>

#include "../Core/Common.hpp"

#include "../Graphics/GPUBuffer.hpp"
#include "../Graphics/GPUDevice.hpp"

namespace TUK::Framework
{
	std::expected<std::unique_ptr<TextureAsset>, std::string> TextureAssetImporter::Import(
		const std::filesystem::path& path) const
	{
		std::string extension = path.extension().string();
		std::ranges::transform(extension, extension.begin(), [](unsigned char character)
		{
			return static_cast<char>(std::tolower(character));
		});

		if (extension != ".dds")
		{
			return std::unexpected("지원하지 않는 이미지 파일 확장자입니다: " + extension);
		}

		auto& d3dDevice = device.GetD3DDevice();
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		std::unique_ptr<uint8_t[]> pixels;
		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		bool isCubeMap = false;
		const HRESULT result = DirectX::LoadDDSTextureFromFile(&d3dDevice, path.c_str(),
			resource.GetAddressOf(), pixels, subresources, 0, nullptr, &isCubeMap);
		if (FAILED(result))
		{
			return std::unexpected("이미지 디코딩에 실패했습니다: " + path.generic_string());
		}

		const auto description = resource->GetDesc();
		if (isCubeMap || description.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || description.DepthOrArraySize != 1)
		{
			return std::unexpected("DDS 파일에 일반 2D 텍스처가 필요합니다.");
		}
		if (subresources.empty())
		{
			return std::unexpected("이미지에 텍스처 서브리소스가 없습니다.");
		}

		const UINT subresourceCount = static_cast<UINT>(subresources.size());
		const UINT64 uploadSize = GetRequiredIntermediateSize(resource.Get(), 0, subresourceCount);
		auto upload = device.CreateBuffer(D3D12_HEAP_TYPE_UPLOAD, uploadSize, D3D12_RESOURCE_STATE_GENERIC_READ);
		if (!upload)
		{
			return std::unexpected(upload.error());
		}

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;
		if (FAILED(d3dDevice.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(allocator.GetAddressOf()))))
		{
			return std::unexpected("이미지 업로드 명령 할당자 생성에 실패했습니다.");
		}
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commands;
		if (FAILED(d3dDevice.CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator.Get(), nullptr,
			IID_PPV_ARGS(commands.GetAddressOf()))))
		{
			return std::unexpected("이미지 업로드 명령 리스트 생성에 실패했습니다.");
		}
		Microsoft::WRL::ComPtr<ID3D12Fence> fence;
		if (FAILED(d3dDevice.CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()))))
		{
			return std::unexpected("이미지 업로드 펜스 생성에 실패했습니다.");
		}

		if (UpdateSubresources(commands.Get(), resource.Get(), &upload->GetD3DResource(),
			0, 0, subresourceCount, subresources.data()) == 0)
		{
			return std::unexpected("업로드 버퍼로 이미지 데이터를 복사하지 못했습니다.");
		}
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		commands->ResourceBarrier(1, &barrier);
		if (FAILED(commands->Close()))
		{
			return std::unexpected("이미지 업로드 명령 리스트 닫기에 실패했습니다.");
		}

		auto& queue = device.GetD3DCommandQueue();
		ID3D12CommandList* lists[] = { commands.Get() };
		queue.ExecuteCommandLists(1, lists);
		constexpr UINT64 COMPLETION_VALUE = 1;
		// A null event makes SetEventOnCompletion block until completion.
		if (FAILED(queue.Signal(fence.Get(), COMPLETION_VALUE)) ||
			FAILED(fence->SetEventOnCompletion(COMPLETION_VALUE, nullptr)))
		{
			if (FAILED(d3dDevice.GetDeviceRemovedReason()))
			{
				return std::unexpected("이미지 업로드 중 디바이스가 제거되었습니다.");
			}
			// Returning would release resources that may still be in use by the GPU.
			LOG_ERROR("이미지 업로드를 동기화할 수 없어 GPU 리소스의 수명을 보장할 수 없습니다.");
			std::abort();
		}
		if (fence->GetCompletedValue() == std::numeric_limits<UINT64>::max())
		{
			return std::unexpected("이미지 업로드 중 디바이스가 제거되었습니다.");
		}

		return std::make_unique<TextureAsset>(path, GPUTexture2D(std::move(resource)));
	}
}
