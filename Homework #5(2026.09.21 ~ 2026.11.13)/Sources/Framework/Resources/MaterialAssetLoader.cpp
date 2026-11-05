#include "Precompiled.hpp"
#include "MaterialAssetLoader.hpp"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace TUK::Framework
{
	std::expected<std::unique_ptr<Asset>, std::string> MaterialAssetLoader::Load(
		const std::filesystem::path& path) const
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
		if (file.bad() || !material->Deserialize(bytes))
		{
			return std::unexpected("Cannot deserialize material: " + path.generic_string());
		}

		return material;
	}
}
