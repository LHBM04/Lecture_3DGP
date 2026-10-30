#include "Precompiled.hpp"

#include "AssetSubsystem.hpp"

namespace TUK::Framework
{
	SystemStatus AssetSubsystem::OnStartup()
	{
		return SystemStatus::Success;
	}

	SystemStatus AssetSubsystem::OnEarlyUpdate()
	{
		return SystemStatus::Continue;
	}

	SystemStatus AssetSubsystem::OnFixedUpdate()
	{
		return SystemStatus::Continue;
	}

	SystemStatus AssetSubsystem::OnUpdate()
	{
		return SystemStatus::Continue;
	}

	SystemStatus AssetSubsystem::OnLateUpdate()
	{
		return SystemStatus::Continue;
	}

	void AssetSubsystem::OnShutdown()
	{
		assets.clear();
		assetLoadersByType.clear();
		assetLoaders.clear();
	}
}
