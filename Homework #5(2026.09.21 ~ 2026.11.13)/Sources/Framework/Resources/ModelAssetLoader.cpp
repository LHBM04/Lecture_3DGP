#include "Precompiled.hpp"
#include "ModelAssetLoader.hpp"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "AssetSubsystem.hpp"
#include "MaterialAsset.hpp"
#include "MeshAsset.hpp"

namespace TUK::Framework
{
	namespace
	{
		[[nodiscard]] std::filesystem::path BuildResourcePath(
			const std::filesystem::path& modelPath,
			std::string_view directory,
			std::string_view name)
		{
			std::filesystem::path resource(std::u8string(name.begin(), name.end()));
			if (!resource.has_parent_path())
			{
				resource = modelPath.parent_path().parent_path() / directory / resource;
			}
			if (resource.extension() != ".bin")
			{
				resource += ".bin";
			}
			return resource.lexically_normal();
		}
	}

	ModelAssetLoader::ModelAssetLoader(AssetSubsystem& assets) noexcept
		: assets(assets)
	{
	}

	ModelAssetLoader::ModelAssetLoader([[maybe_unused]] const GPUDevice& device, AssetSubsystem& assets) noexcept
		: assets(assets)
	{
	}

	std::expected<std::unique_ptr<Asset>, std::string> ModelAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		if (path.extension() != ".bin")
		{
			return std::unexpected("ModelAssetLoader requires a .bin model file.");
		}

		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return std::unexpected("Cannot open model: " + path.generic_string());
		}

		const std::string bytes{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
		auto model = std::make_unique<ModelAsset>();
		if (file.bad() || !model->Deserialize(bytes))
		{
			return std::unexpected("Cannot deserialize model: " + path.generic_string());
		}

		for (ModelNode& node : model->GetNodes())
		{
			for (ModelRendererReference& renderer : node.renderers)
			{
				const MeshAsset* mesh = assets.LoadAsset<MeshAsset>(
					BuildResourcePath(path, "Meshes", renderer.meshName));
				const MaterialAsset* material = assets.LoadAsset<MaterialAsset>(
					BuildResourcePath(path, "Materials", renderer.materialName));
				if (!mesh || !material)
				{
					return std::unexpected("Cannot load dependencies for model: " + path.generic_string());
				}

				renderer.mesh = mesh;
				renderer.material = material;
			}
		}

		return model;
	}
}
