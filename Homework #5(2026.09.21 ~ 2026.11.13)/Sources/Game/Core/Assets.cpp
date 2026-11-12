#include "Precompiled.hpp"
#include "Assets.hpp"

namespace TUK::Game
{
	namespace
	{
		AssetSubsystem* assetSubsystem = nullptr;
	}

	namespace Assets
	{
		void Initialize(AssetSubsystem* subsystem) noexcept
		{
			assetSubsystem = subsystem;
		}

		void Release() noexcept
		{
			assetSubsystem = nullptr;
		}

		AssetSubsystem* GetSubsystem() noexcept
		{
			return assetSubsystem;
		}

		void UnloadAsset(const std::filesystem::path& path)
		{
			if (assetSubsystem)
			{
				assetSubsystem->UnloadAsset(path);
			}
		}

		void UnloadAllAssets() noexcept
		{
			if (assetSubsystem)
			{
				assetSubsystem->UnloadAllAssets();
			}
		}
	}
}
