#include "Precompiled.hpp"
#include "ModelAssetLoader.hpp"

#include <filesystem>
#include <format>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "AssetSubsystem.hpp"
#include "MaterialAsset.hpp"
#include "MeshAsset.hpp"
#include "ResourceImporter.hpp"

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
		if (path.extension() != ".tukmodel")
		{
			return std::unexpected("모델 로더에는 .tukmodel 파일이 필요합니다.");
		}
		auto metadata = ResourceImporter::Load<ModelMetadata>(path, ResourceType::MODEL);
		if (!metadata)
		{
			return std::unexpected(metadata.error());
		}
		const auto root = ResourceMetadata::FindRoot(path, "Models");
		if (!root)
		{
			return std::unexpected(root.error());
		}
		struct PendingNode
		{
			const ModelNodeMetadata* data;
			i32 parentIndex;
		};
		std::vector<PendingNode> pending{ { &metadata->root, -1 } };
		std::vector<Submodel> nodes;
		while (!pending.empty())
		{
			const auto [data, parentIndex] = pending.back();
			pending.pop_back();
			if (nodes.size() >= SerializationLimits::MAX_ELEMENTS)
			{
				return std::unexpected("모델의 노드 수가 제한을 초과했습니다.");
			}
			const i32 index = static_cast<i32>(nodes.size());
			Submodel node{};
			node.name = data->name;
			node.parentIndex = parentIndex;
			node.localPosition.Set(data->localPosition[0], data->localPosition[1], data->localPosition[2]);
			node.localRotation.Set(data->localRotation[0], data->localRotation[1], data->localRotation[2], data->localRotation[3]);
			node.localScale.Set(data->localScale[0], data->localScale[1], data->localScale[2]);
			for (const auto& part : data->renderParts)
			{
				const auto meshPath = ResourceMetadata::ResolvePath(*root, part.mesh);
				const auto materialPath = ResourceMetadata::ResolvePath(*root, part.material);
				if (!meshPath || !materialPath)
				{
					return std::unexpected("모델의 리소스 참조 경로가 유효하지 않습니다.");
				}
				const MeshAsset* mesh = assets->LoadAsset<MeshAsset>(*meshPath);
				const MaterialAsset* material = assets->LoadAsset<MaterialAsset>(*materialPath);
				if (!mesh || !material)
				{
					return std::unexpected(std::format("모델의 렌더 파트를 로드하지 못했습니다: {}, {}",
						meshPath->generic_string(), materialPath->generic_string()));
				}
				node.renderParts.push_back({ mesh, material });
			}
			nodes.push_back(std::move(node));
			for (auto child = data->children.crbegin(); child != data->children.crend(); ++child)
			{
				pending.push_back({ &*child, index });
			}
		}
		return std::make_unique<ModelAsset>(path, std::move(nodes));
	}
}
