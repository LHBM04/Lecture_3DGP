#pragma once

#include <expected>
#include <filesystem>
#include <memory>
#include <string>

#include "AssetLoader.hpp"
#include "TextureAsset.hpp"

namespace TUK::Framework
{
	class AssetSubsystem;
	class GPUDevice;

	class TextureAssetLoader final : public AssetLoader<TextureAsset>
	{
	public:
		explicit TextureAssetLoader(AssetSubsystem* owner, const GPUDevice& device) noexcept;

		[[nodiscard]] std::expected<std::unique_ptr<TextureAsset>, std::string> Load(
			const std::filesystem::path& path) const override;

	private:
		const GPUDevice& device;
	};

	inline TextureAssetLoader::TextureAssetLoader(AssetSubsystem* owner, const GPUDevice& device) noexcept
		: AssetLoader<TextureAsset>(owner)
		, device(device)
	{
	}
}
