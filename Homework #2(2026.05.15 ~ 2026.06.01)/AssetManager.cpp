#include "Precompiled.h"
#include "AssetManager.h"

void AssetManager::Unload(Asset* asset_) noexcept
{
	if (nullptr == asset_)
	{
		return;
	}

	const auto iterator{ std::find_if(
		assets.begin(),
		assets.end(),
		[asset_](const std::unique_ptr<Asset>& asset)
		{
			return asset.get() == asset_;
		}) };

	if (iterator == assets.end())
	{
		return;
	}

	(*iterator)->Unload();
	assets.erase(iterator);
}

void AssetManager::Clear() noexcept
{
	for (std::unique_ptr<Asset>& asset : assets)
	{
		if (nullptr != asset)
		{
			asset->Unload();
		}
	}

	assets.clear();
}
