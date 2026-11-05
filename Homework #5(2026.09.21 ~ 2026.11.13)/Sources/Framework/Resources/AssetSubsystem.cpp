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
		const auto preload = [this]<class TAsset>(const std::filesystem::path& directory, std::string_view extension)
		{
			std::error_code error;
			std::filesystem::recursive_directory_iterator entry(directory, error);
			const std::filesystem::recursive_directory_iterator END;
			for (; !error && entry != END; entry.increment(error))
			{
				if (entry->is_regular_file(error) && entry->path().extension() == extension
					&& !LoadAsset<TAsset>(entry->path()))
				{
					return false;
				}
			}
			if (error)
			{
				std::cerr << "Cannot scan assets: " << directory.generic_string() << ": " << error.message() << '\n';
				return false;
			}
			return true;
		};

		if (!preload.operator()<ShaderAsset>("Resources/Shaders", ".cso")
			|| !preload.operator()<MaterialAsset>("Resources/Materials", ".bin"))
		{
			return SystemStatus::FAILURE;
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
