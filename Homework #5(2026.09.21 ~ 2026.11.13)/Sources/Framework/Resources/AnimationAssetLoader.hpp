#pragma once

#include "AssetLoader.hpp"
#include "AnimationAsset.hpp"

namespace TUK::Framework
{
	class AnimationAssetLoader final : public AssetLoader<AnimationAsset>
	{
	public:
		explicit AnimationAssetLoader(AssetSubsystem* owner) noexcept;
		[[nodiscard]] std::expected<std::unique_ptr<AnimationAsset>, std::string> Load(
			const std::filesystem::path& path) const override;
	};

	inline AnimationAssetLoader::AnimationAssetLoader(AssetSubsystem* owner) noexcept
		: AssetLoader<AnimationAsset>(owner)
	{
	}
}
