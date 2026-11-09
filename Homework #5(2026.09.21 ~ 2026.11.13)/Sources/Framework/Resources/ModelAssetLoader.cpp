#include "Precompiled.hpp"
#include "ModelAssetLoader.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

#include "AssetSubsystem.hpp"
#include "MaterialAsset.hpp"
#include "MeshAsset.hpp"

namespace TUK::Framework
{
	std::expected<std::unique_ptr<ModelAsset>, std::string> ModelAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		AssetSubsystem* assets = GetOwner();
		if (!assets)
		{
			return std::unexpected("모델 로더의 에셋 서브시스템이 nullptr입니다.");
		}

		if (path.extension() != ".json")
		{
			return std::unexpected("모델 로더에는 .json 파일이 필요합니다.");
		}

		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return std::unexpected("모델 파일을 열지 못했습니다: " + path.generic_string());
		}

		const auto parseError = std::unexpected("모델 JSON을 읽지 못했습니다: " + path.generic_string());
		const auto json = nlohmann::json::parse(file, nullptr, false);
		if (file.bad() || !json.is_object())
		{
			return parseError;
		}

	constexpr u32 MAX_ELEMENT_COUNT = 16 * 1024 * 1024;

	struct PendingNode
	{
		const nlohmann::json* json;
		i32 parentIndex;
	};

	struct PendingRenderPart
	{
		usize nodeIndex;
		std::filesystem::path meshPath;
		std::filesystem::path materialPath;
	};

		std::vector<Submodel> nodes;
		std::vector<PendingRenderPart> renderParts;
		std::vector<PendingNode> pending{ { &json, -1 } };
		while (!pending.empty())
		{
			const auto [value, parentIndex] = pending.back();
			pending.pop_back();
			if (!value->is_object() || nodes.size() >= MAX_ELEMENT_COUNT)
			{
				return parseError;
			}

			const auto name = value->find("name");
			const auto position = value->find("localPosition");
			const auto rotation = value->find("localRotation");
			const auto scale = value->find("localScale");
			const auto parts = value->find("renderParts");
			const auto children = value->find("children");
			Submodel node{};
			if (name == value->end() || !name->is_string()
				|| position == value->end() || !node.localPosition.Deserialize(*position)
				|| rotation == value->end() || !node.localRotation.Deserialize(*rotation)
				|| scale == value->end() || !node.localScale.Deserialize(*scale)
				|| parts == value->end() || !parts->is_array() || parts->size() > MAX_ELEMENT_COUNT
				|| children == value->end() || !children->is_array() || children->size() > MAX_ELEMENT_COUNT)
			{
				return parseError;
			}

			const usize index = nodes.size();
			node.name = name->get<std::string>();
			node.parentIndex = parentIndex;
			node.renderParts.reserve(parts->size());
			for (const auto& part : *parts)
			{
				PendingRenderPart renderPart{ .nodeIndex = index };
				const std::pair<const char*, std::filesystem::path&> pathFields[]{
					{ "mesh", renderPart.meshPath },
					{ "material", renderPart.materialPath }
				};
				for (const auto& [key, outPath] : pathFields)
				{
					const auto field = part.find(key);
					if (field == part.end() || !field->is_string())
					{
						return parseError;
					}

					const auto& name = field->get_ref<const std::string&>();
					if (name.empty() || name.find('\0') != std::string::npos)
					{
						return parseError;
					}

					auto assetPath = std::filesystem::path(std::u8string(name.begin(), name.end())).lexically_normal();
					if (assetPath.has_root_path() || assetPath.empty() || *assetPath.begin() == ".."
						|| assetPath.extension() != ".json")
					{
						return parseError;
					}
					outPath = std::move(assetPath);
				}
				renderParts.push_back(std::move(renderPart));
			}

			nodes.push_back(std::move(node));
			for (auto child = children->crbegin(); child != children->crend(); ++child)
			{
				pending.push_back({ &*child, static_cast<i32>(index) });
			}
		}

		if (renderParts.empty())
		{
			return parseError;
		}

		const auto modelDir = path.parent_path();
		auto root = modelDir.parent_path();
		if (modelDir.filename() != "Models" && root.filename() == "Models")
		{
			root = root.parent_path();
		}
		for (const auto& part : renderParts)
		{
			const auto meshPath = root / part.meshPath;
			const MeshAsset* mesh = assets->LoadAsset<MeshAsset>(meshPath);
			if (!mesh)
			{
				return std::unexpected(std::format("모델의 메시 에셋을 로드하지 못했습니다: {}", meshPath.generic_string()));
			}

			const auto materialPath = root / part.materialPath;
			const MaterialAsset* material = assets->LoadAsset<MaterialAsset>(materialPath);
			if (!material)
			{
				return std::unexpected(std::format("모델의 머티리얼 에셋을 로드하지 못했습니다: {}", materialPath.generic_string()));
			}

			nodes[part.nodeIndex].renderParts.push_back({ mesh, material });
		}

		return std::make_unique<ModelAsset>(path, std::move(nodes));
	}
}
