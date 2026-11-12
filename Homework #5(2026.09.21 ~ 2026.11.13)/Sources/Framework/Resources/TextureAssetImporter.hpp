#pragma once

#include "AssetImporter.hpp"
#include "TextureAsset.hpp"

namespace TUK::Framework
{
	class AssetSubsystem;
	class GPUDevice;

	class TextureAssetImporter final : public AssetImporter<TextureAsset>
	{
	public:
		explicit TextureAssetImporter(AssetSubsystem* owner, const GPUDevice& device) noexcept;

		[[nodiscard]] std::expected<std::unique_ptr<TextureAsset>, std::string> Import(
			const std::filesystem::path& path) const override;

	private:
		const GPUDevice& device;
	};

	inline TextureAssetImporter::TextureAssetImporter(AssetSubsystem* owner, const GPUDevice& device) noexcept
		: AssetImporter<TextureAsset>(owner)
		, device(device)
	{
	}
}
