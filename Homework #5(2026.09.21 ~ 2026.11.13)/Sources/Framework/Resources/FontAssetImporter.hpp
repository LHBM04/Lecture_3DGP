#pragma once

#include "AssetImporter.hpp"
#include "FontAsset.hpp"

namespace TUK::Framework
{
	class FontAssetImporter final : public AssetImporter<FontAsset>
	{
	public:
		explicit FontAssetImporter(AssetSubsystem* owner) noexcept;
		[[nodiscard]] std::expected<std::unique_ptr<FontAsset>, std::string> Import(
			const std::filesystem::path& path) const override;
	};

	inline FontAssetImporter::FontAssetImporter(AssetSubsystem* owner) noexcept
		: AssetImporter<FontAsset>(owner)
	{
	}
}
