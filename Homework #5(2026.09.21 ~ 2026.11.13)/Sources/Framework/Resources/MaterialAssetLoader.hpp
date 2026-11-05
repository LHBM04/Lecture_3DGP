#pragma once

#include "AssetLoader.hpp"
#include "MaterialAsset.hpp"

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class AssetSubsystem;
	class GPUDevice;

	class MaterialAssetLoader final : public AssetLoader
	{
	public:
		using AssetType = MaterialAsset;

		MaterialAssetLoader(const AssetSubsystem& assets, const GPUDevice& device, DXGI_FORMAT targetFormat) noexcept;
		~MaterialAssetLoader() noexcept override = default;

		[[nodiscard]] std::expected<std::unique_ptr<Asset>, std::string> Load(
			const std::filesystem::path& path) const override;

	private:
		const AssetSubsystem& assets;
		const GPUDevice& device;
		DXGI_FORMAT targetFormat;
	};
}
