#pragma once

#include <d3d12.h>

#include "AssetLoader.hpp"
#include "MaterialAsset.hpp"

namespace TUK::Framework
{
	class AssetSubsystem;
	class GPUDevice;

	class MaterialAssetLoader final : public AssetLoader
	{
	public:
		using AssetType = MaterialAsset;

		MaterialAssetLoader(
			const GPUDevice& device, AssetSubsystem& assets,
			DXGI_FORMAT targetFormat = DXGI_FORMAT_R8G8B8A8_UNORM) noexcept;
		~MaterialAssetLoader() noexcept override = default;

		[[nodiscard]] std::expected<std::unique_ptr<Asset>, std::string> Load(
			const std::filesystem::path& path) const override;

	private:
		const GPUDevice& device;
		AssetSubsystem& assets;
		DXGI_FORMAT targetFormat;
	};
}
