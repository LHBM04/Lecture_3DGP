#include "Precompiled.hpp"
#include "AssetSubsystem.hpp"

namespace TUK::Framework
{
	void AssetSubsystem::UnloadAsset(const std::filesystem::path& path)
	{
		const std::filesystem::path normalizedPath = path.lexically_normal();
		const auto cached = assetsByPath.find(normalizedPath);
		if (cached == assetsByPath.end())
		{
			return;
		}

		const Asset* target = cached->second;
		assetsByPath.erase(cached);
		std::erase_if(assets, [target](const std::unique_ptr<Asset>& asset)
		{
			return asset.get() == target;
		});
	}

	void AssetSubsystem::UnloadAllAssets() noexcept
	{
		assetsByPath.clear();
		assets.clear();
	}

	void AssetSubsystem::OnWorldEntered([[maybe_unused]] const World* world)
	{
	}

	void AssetSubsystem::OnWorldExited([[maybe_unused]] const World* world)
	{
		UnloadAllAssets();
	}

	SystemStatus AssetSubsystem::OnStartup()
	{
		return SystemStatus::CONTINUE;
	}

	SystemStatus AssetSubsystem::OnEarlyUpdate()
	{
		return SystemStatus::CONTINUE;
	}

	SystemStatus AssetSubsystem::OnFixedUpdate()
	{
		return SystemStatus::CONTINUE;
	}

	SystemStatus AssetSubsystem::OnUpdate()
	{
		return SystemStatus::CONTINUE;
	}

	SystemStatus AssetSubsystem::OnLateUpdate()
	{
		return SystemStatus::CONTINUE;
	}

	void AssetSubsystem::OnShutdown()
	{
		UnloadAllAssets();
		assetLoadersByType.clear();
		assetLoaders.clear();
	}
}
