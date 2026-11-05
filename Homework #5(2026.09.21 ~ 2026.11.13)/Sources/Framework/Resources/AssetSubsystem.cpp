#include "Precompiled.hpp"
#include "AssetSubsystem.hpp"

#include "MaterialAsset.hpp"
#include "MeshAsset.hpp"
#include "ShaderAsset.hpp"
#include "ModelAsset.hpp"

namespace TUK::Framework
{
	SystemStatus AssetSubsystem::OnStartup()
	{
		std::filesystem::path materialAssetsPath = "Resources/Materials";
		for (const auto& entry : std::filesystem::recursive_directory_iterator(materialAssetsPath))
		{
			if (entry.path().extension() == ".bin")
			{	
				LoadAsset<MaterialAsset>(entry.path());
			}
		}

		std::filesystem::path meshesAssetsPath = "Resources/Meshes";
		for (const auto& entry : std::filesystem::recursive_directory_iterator(meshesAssetsPath))
		{
			if (entry.path().extension() == ".bin")
			{	
				LoadAsset<MeshAsset>(entry.path());
			}
		}

		std::filesystem::path modelsAssetsPath = "Resources/Models";
		for (const auto& entry : std::filesystem::recursive_directory_iterator(modelsAssetsPath))
		{
			if (entry.path().extension() == ".bin")
			{	
				LoadAsset<ModelAsset>(entry.path());
			}
		}

		std::filesystem::path shaderAssetsPath = "Resources/Shaders";
		for (const auto& entry : std::filesystem::recursive_directory_iterator(shaderAssetsPath))
		{
			if (entry.path().extension() == ".cso")
			{	
				LoadAsset<ShaderAsset>(entry.path());
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
