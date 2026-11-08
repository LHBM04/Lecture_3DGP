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

	class ImageAssetLoader final : public AssetLoader<ImageAsset>
	{
	public:
		explicit ImageAssetLoader(const GPUDevice& device) noexcept;

		[[nodiscard]] std::expected<std::unique_ptr<ImageAsset>, std::string> Load(
			const std::filesystem::path& path) const override;

	private:
		const GPUDevice& device;
	};
}
