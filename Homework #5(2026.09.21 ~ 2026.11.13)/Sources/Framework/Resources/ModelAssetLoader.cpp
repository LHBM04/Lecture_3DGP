#include "Precompiled.hpp"
#include "ModelAssetLoader.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <istream>
#include <limits>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <iterator>
#include <vector>

#include "../Graphics/GPUBuffer.hpp"
#include "../Graphics/GPUDevice.hpp"

#include "../Math/Quaternion.hpp"
#include "../Math/Vector3D.hpp"

#include "../Utilities/BinaryUtility.hpp"

#include "AssetSubsystem.hpp"
#include "MaterialAsset.hpp"

namespace TUK::Framework
{
	ModelAssetLoader::ModelAssetLoader(const GPUDevice& device, AssetSubsystem& assets) noexcept
		: device(device)
		, assets(assets)
	{
	}

	std::expected<std::unique_ptr<Asset>, std::string> ModelAssetLoader::Load(const std::filesystem::path& path) const
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
		Asset& asset = *model;
		if (file.bad() || !asset.Deserialize(bytes) || !model->Upload(device, assets, path))
		{
			return std::unexpected("Cannot deserialize or upload model: " + path.generic_string());
		}
		return model;
	}
}
