#include "Precompiled.hpp"
#include "ImageAssetLoader.hpp"

#include "../Graphics/GPUBuffer.hpp"
#include "../Graphics/GPUDevice.hpp"

#include "../Utilities/Logger.hpp"

namespace TUK::Framework
{
	ImageAssetLoader::ImageAssetLoader(const GPUDevice& device) noexcept
		: device(device)
	{
	}

	std::expected<std::unique_ptr<Asset>, std::string> ImageAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		std::string extension = path.extension().string();
		std::ranges::transform(extension, extension.begin(), [](unsigned char character)
		{
			return static_cast<char>(std::tolower(character));
		});

		const bool isDDS = extension == ".dds";
		if (!isDDS && extension != ".png" && extension != ".jpg" && extension != ".jpeg" && extension != ".bmp")
		{
			return std::unexpected("Unsupported image file extension: " + extension);
		}

		auto& d3dDevice = device.GetD3DDevice();
		ComPtr<ID3D12Resource> resource;
		std::unique_ptr<uint8_t[]> pixels;
		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		bool isCubeMap = false;
		HRESULT result;
		if (isDDS)
		{
			result = DirectX::LoadDDSTextureFromFile(&d3dDevice, path.c_str(),
				resource.GetAddressOf(), pixels, subresources, 0, nullptr, &isCubeMap);
		}
		else
		{
			const HRESULT initialization = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
			if (FAILED(initialization) && initialization != RPC_E_CHANGED_MODE)
			{
				return std::unexpected("Failed to initialize COM for image decoding.");
			}
			D3D12_SUBRESOURCE_DATA subresource{};
			result = DirectX::LoadWICTextureFromFile(&d3dDevice, path.c_str(),
				resource.GetAddressOf(), pixels, subresource);
			if (SUCCEEDED(initialization))
			{
				CoUninitialize();
			}
			subresources.push_back(subresource);
		}
		if (FAILED(result))
		{
			return std::unexpected("Failed to decode image: " + path.generic_string());
		}

		const auto description = resource->GetDesc();
		if (isCubeMap || description.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || description.DepthOrArraySize != 1)
		{
			return std::unexpected("ImageAssetLoader supports only non-array 2D textures.");
		}
		if (subresources.empty())
		{
			return std::unexpected("Image contains no texture subresources.");
		}

		const UINT subresourceCount = static_cast<UINT>(subresources.size());
		const UINT64 uploadSize = GetRequiredIntermediateSize(resource.Get(), 0, subresourceCount);
		auto upload = GPUBuffer::Create(device, D3D12_HEAP_TYPE_UPLOAD, uploadSize, D3D12_RESOURCE_STATE_GENERIC_READ);
		if (!upload)
		{
			return std::unexpected(upload.error());
		}

		ComPtr<ID3D12CommandAllocator> allocator;
		if (FAILED(d3dDevice.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(allocator.GetAddressOf()))))
		{
			return std::unexpected("Failed to create image upload command allocator.");
		}
		ComPtr<ID3D12GraphicsCommandList> commands;
		if (FAILED(d3dDevice.CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator.Get(), nullptr,
			IID_PPV_ARGS(commands.GetAddressOf()))))
		{
			return std::unexpected("Failed to create image upload command list.");
		}
		ComPtr<ID3D12Fence> fence;
		if (FAILED(d3dDevice.CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()))))
		{
			return std::unexpected("Failed to create image upload fence.");
		}

		if (UpdateSubresources(commands.Get(), resource.Get(), &upload->GetD3DResource(),
			0, 0, subresourceCount, subresources.data()) == 0)
		{
			return std::unexpected("Failed to copy image data into upload buffer.");
		}
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		commands->ResourceBarrier(1, &barrier);
		if (FAILED(commands->Close()))
		{
			return std::unexpected("Failed to close image upload command list.");
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
				return std::unexpected("Device removed during image upload.");
			}
			// Returning would release resources that may still be in use by the GPU.
			Logger::LogError("Cannot synchronize image upload; GPU resource lifetime cannot be guaranteed.");
			std::abort();
		}
		if (fence->GetCompletedValue() == std::numeric_limits<UINT64>::max())
		{
			return std::unexpected("Device removed during image upload.");
		}

		auto image = std::make_unique<ImageAsset>();
		image->SetTexture(GPUTexture2D(std::move(resource)));
		return image;
	}
}
