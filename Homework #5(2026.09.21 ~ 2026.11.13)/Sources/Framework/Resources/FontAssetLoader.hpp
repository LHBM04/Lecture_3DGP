#pragma once

#include "AssetLoader.hpp"
#include "FontAsset.hpp"

namespace TUK::Framework
{
	class FontAssetLoader final : public AssetLoader<FontAsset>
	{
	public:
		explicit FontAssetLoader(AssetSubsystem* owner) noexcept;
		[[nodiscard]] std::expected<std::unique_ptr<FontAsset>, std::string> Load(
			const std::filesystem::path& path) const override;
	};

	inline FontAssetLoader::FontAssetLoader(AssetSubsystem* owner) noexcept
		: AssetLoader<FontAsset>(owner)
	{
	}
}
