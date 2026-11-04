#include "Precompiled.hpp"
#include "MaterialAssetLoader.hpp"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <istream>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <iterator>

#include "../Utilities/BinaryUtility.hpp"

#include "AssetSubsystem.hpp"
#include "ShaderAsset.hpp"

namespace TUK::Framework
{
	MaterialAssetLoader::MaterialAssetLoader(const GPUDevice& device, AssetSubsystem& assets, DXGI_FORMAT targetFormat) noexcept
		: device(device)
		, assets(assets)
		, targetFormat(targetFormat)
	{
	}

	std::expected<std::unique_ptr<Asset>, std::string> MaterialAssetLoader::Load(const std::filesystem::path& path) const
	{
		if (path.extension() != ".bin")
		{
			return std::unexpected("MaterialAssetLoader requires a .bin file.");
		}
		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return std::unexpected("Cannot open material: " + path.generic_string());
		}
		const std::string bytes{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
		auto material = std::make_unique<MaterialAsset>();
		Asset& asset = *material;
		if (file.bad() || !asset.Deserialize(bytes) || !material->Upload(device, assets, path, targetFormat))
		{
			return std::unexpected("Cannot deserialize or upload material: " + path.generic_string());
		}
		return material;
	}
}
