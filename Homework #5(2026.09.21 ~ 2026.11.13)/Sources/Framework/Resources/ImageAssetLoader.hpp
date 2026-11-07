#pragma once

#include <expected>
#include <filesystem>
#include <memory>
#include <string>

#include "AssetLoader.hpp"
#include "ImageAsset.hpp"

namespace TUK::Framework
{
	class GPUDevice;

	class ImageAssetLoader final : public AssetLoader
	{
	public:
		using AssetType = ImageAsset;

		explicit ImageAssetLoader(const GPUDevice& device) noexcept;

		[[nodiscard]] std::expected<std::unique_ptr<Asset>, std::string> Load(
			const std::filesystem::path& path) const override;

	private:
		const GPUDevice& device;
	};
}
