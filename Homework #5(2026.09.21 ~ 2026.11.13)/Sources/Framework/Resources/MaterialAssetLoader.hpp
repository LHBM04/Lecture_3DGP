#pragma once

#include "../Core/Common.hpp"

#include "AssetLoader.hpp"
#include "MaterialAsset.hpp"

namespace TUK::Framework
{
	class AssetSubsystem;
	class GPUDevice;

	class MaterialAssetLoader final : public AssetLoader<MaterialAsset>
	{
	public:
		MaterialAssetLoader(AssetSubsystem* assets, const GPUDevice& device, DXGI_FORMAT targetFormat) noexcept;

		[[nodiscard]] std::expected<std::unique_ptr<MaterialAsset>, std::string> Load(
			const std::filesystem::path& path) const override;

	private:
		AssetSubsystem* assets = nullptr;
		const GPUDevice& device;
		DXGI_FORMAT targetFormat;
	};
}
