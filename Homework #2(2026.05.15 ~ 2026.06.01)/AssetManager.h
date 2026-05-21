#pragma once

#include "Asset.h"

class AssetManager final
{
	STATIC_CLASS(AssetManager);

public:
	template <std::derived_from<Asset> TAsset>
	[[nodiscard]] static TAsset* Load(const std::filesystem::path& path_);
	static void Unload(Asset* asset_) noexcept;

	static void Clear() noexcept;

private:
	static inline std::vector<std::unique_ptr<Asset>> assets;
};

template <std::derived_from<Asset> TAsset>
TAsset* AssetManager::Load(const std::filesystem::path& path_)
{
	std::unique_ptr<TAsset> asset{ std::make_unique<TAsset>() };
	if (!asset->Load(path_))
	{
		return nullptr;
	}

	TAsset* const loadedAsset{ asset.get() };
	assets.emplace_back(std::move(asset));
	return loadedAsset;
}
