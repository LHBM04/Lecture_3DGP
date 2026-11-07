#include "Precompiled.hpp"
#include "ModelAssetLoader.hpp"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace TUK::Framework
{
	std::expected<std::unique_ptr<Asset>, std::string> ModelAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		std::ifstream file(path, std::ios::binary);
		if (not file)
		{
			return std::unexpected("모델 파일을 열지 못했습니다: " + path.generic_string());
		}

		const std::string bytes{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };

		std::unique_ptr<ModelAsset> model = std::make_unique<ModelAsset>();
		if (!model->Deserialize(bytes))
		{
			return std::unexpected("모델 역직렬화에 실패했습니다: " + path.generic_string());
		}

		return model;
	}
}
