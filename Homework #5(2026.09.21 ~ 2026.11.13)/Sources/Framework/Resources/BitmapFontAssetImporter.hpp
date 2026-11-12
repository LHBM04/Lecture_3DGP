#pragma once

#include "AssetImporter.hpp"
#include "BitmapFontAsset.hpp"

namespace TUK::Framework
{
	class BitmapFontAssetImporter final : public AssetImporter<BitmapFontAsset>
	{
	public:
		explicit BitmapFontAssetImporter(AssetSubsystem* owner) noexcept;
		[[nodiscard]] std::expected<std::unique_ptr<BitmapFontAsset>, std::string> Import(
			const std::filesystem::path& path) const override;
	};

	inline BitmapFontAssetImporter::BitmapFontAssetImporter(AssetSubsystem* owner) noexcept
		: AssetImporter<BitmapFontAsset>(owner)
	{
	}
}
