#include "Precompiled.hpp"

#include "AssetSubsystem.hpp"

namespace TUK::Framework
{
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
		assets.clear();
		assetLoadersByType.clear();
		assetLoaders.clear();
	}
}
