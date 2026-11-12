#pragma once

#include "AssetImporter.hpp"
#include "AnimationAsset.hpp"

namespace TUK::Framework
{
	class AnimationAssetImporter final : public AssetImporter<AnimationAsset>
	{
	public:
		explicit AnimationAssetImporter(AssetSubsystem* owner) noexcept;
		[[nodiscard]] std::expected<std::unique_ptr<AnimationAsset>, std::string> Import(
			const std::filesystem::path& path) const override;
	};

	inline AnimationAssetImporter::AnimationAssetImporter(AssetSubsystem* owner) noexcept
		: AssetImporter<AnimationAsset>(owner)
	{
	}
}
