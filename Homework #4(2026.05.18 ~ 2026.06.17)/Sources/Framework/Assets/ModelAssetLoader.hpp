#pragma once

#include "AssetLoader.hpp"
#include "ModelAsset.hpp"

namespace TUK::Framework
{
	class GPUDevice;

	class ModelAssetLoader final : public AssetLoader
	{
	public:
		using AssetType = ModelAsset;

		explicit ModelAssetLoader(const GPUDevice& device) noexcept;
		~ModelAssetLoader() noexcept override = default;

		[[nodiscard]] std::expected<std::unique_ptr<Asset>, std::string> Load(
			const std::filesystem::path& path) const override;

	private:
		const GPUDevice& device;
	};
}
