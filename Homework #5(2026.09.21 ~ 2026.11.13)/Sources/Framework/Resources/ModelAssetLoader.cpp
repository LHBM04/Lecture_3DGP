#include "Precompiled.hpp"
#include "ModelAssetLoader.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "../Utilities/Logger.hpp"

#include "AssetSubsystem.hpp"
#include "MaterialAsset.hpp"
#include "MeshAsset.hpp"

namespace TUK::Framework
{
	ModelAssetLoader::ModelAssetLoader(AssetSubsystem* assets) noexcept
		: assets(assets)
	{
		ASSERT_FMT(assets, "모델 로더의 에셋 서브시스템이 nullptr입니다.");
	}

	std::expected<std::unique_ptr<ModelAsset>, std::string> ModelAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		if (!assets)
		{
			return std::unexpected("모델 로더의 에셋 서브시스템이 nullptr입니다.");
		}

		std::ifstream file(path, std::ios::binary);
		if (not file)
		{
			return std::unexpected("모델 파일을 열지 못했습니다: " + path.generic_string());
		}

		const std::string bytes{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };

		std::unique_ptr<ModelAsset> model = std::make_unique<ModelAsset>(path);
		if (!model->Deserialize(bytes))
		{
			return std::unexpected("모델 역직렬화에 실패했습니다: " + path.generic_string());
		}

		if (model->GetName().empty())
		{
			model->SetName(path.stem().string());
		}

		const auto resourcePath = [&path](std::string_view directory, std::string_view name)
		{
			std::filesystem::path targetPath(std::u8string(name.begin(), name.end()));
			if (!targetPath.has_parent_path())
			{
				targetPath = path.parent_path().parent_path() / directory / targetPath;
			}
			if (targetPath.extension() != ".bin")
			{
				targetPath += ".bin";
			}
			return targetPath.lexically_normal();
		};

		for (const Submodel& submodel : model->GetSubmodels())
		{
			for (const Submodel::RenderPart& part : submodel.renderParts)
			{
				const auto meshPath = resourcePath("Meshes", part.meshName);
				if (!assets->LoadAsset<MeshAsset>(meshPath))
				{
					return std::unexpected(std::format("모델의 메시 에셋을 로드하지 못했습니다: {}", meshPath.generic_string()));
				}

				const auto materialPath = resourcePath("Materials", part.materialName);
				if (!assets->LoadAsset<MaterialAsset>(materialPath))
				{
					return std::unexpected(std::format("모델의 머티리얼 에셋을 로드하지 못했습니다: {}", materialPath.generic_string()));
				}
			}
		}

		return model;
	}
}
