#include "Precompiled.hpp"
#include <system_error>

#include "AssetSubsystem.hpp"

#include "MaterialAsset.hpp"
#include "MeshAsset.hpp"
#include "ShaderAsset.hpp"
#include "ModelAsset.hpp"

namespace TUK::Framework
{
	SystemStatus AssetSubsystem::OnStartup()
	{
		std::filesystem::path shadersAssetsPath = "Resources/Shaders";
		for (const auto& entry : std::filesystem::recursive_directory_iterator(shadersAssetsPath))
		{
			if (entry.path().extension() == ".cso")
			{	
				LoadAsset<ShaderAsset>(entry.path());
				Logger::LogTrace("{}", entry.path().generic_string());
			}
		}

		std::filesystem::path materialsAssetsPath = "Resources/Materials";
		for (const auto& entry : std::filesystem::recursive_directory_iterator(materialsAssetsPath))
		{
			if (entry.path().extension() == ".bin")
			{	
				LoadAsset<MaterialAsset>(entry.path());
				Logger::LogTrace("{}", entry.path().generic_string());
			}
		}

		std::filesystem::path meshesAssetsPath = "Resources/Meshes";
		for (const auto& entry : std::filesystem::recursive_directory_iterator(meshesAssetsPath))
		{
			if (entry.path().extension() == ".bin")
			{	
				LoadAsset<MeshAsset>(entry.path());
				Logger::LogTrace("{}", entry.path().generic_string());
			}
		}

		std::filesystem::path modelsAssetsPath = "Resources/Models";
		for (const auto& entry : std::filesystem::recursive_directory_iterator(modelsAssetsPath))
		{
			if (entry.path().extension() == ".bin")
			{	
				LoadAsset<ModelAsset>(entry.path());
				Logger::LogTrace("{}", entry.path().generic_string());
			}
		}

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
		assetsByPath.clear();
		assets.clear();
		assetLoadersByType.clear();
		assetLoaders.clear();
	}
}
