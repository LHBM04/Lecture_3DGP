#include "Precompiled.hpp"
#include "ModelAssetImporter.hpp"

#include "../Core/BinaryReader.hpp"

#include "AssetSubsystem.hpp"
#include "MaterialAsset.hpp"
#include "MeshAsset.hpp"
#include "ResourcePath.hpp"

namespace TUK::Framework
{
	std::expected<std::unique_ptr<ModelAsset>, std::string> ModelAssetImporter::Import(
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

		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return std::unexpected("리소스를 열 수 없습니다: " + path.generic_string());
		}

		BinaryReader reader(file);
		const auto root = FindResourceRoot(path, "Models");
		if (!root)
		{
			return std::unexpected(root.error());
		}

		struct PendingChildren final
		{
			i32 parentIndex;
			i32 count;
		};

		std::vector<PendingChildren> pending{ { -1, 1 } };
		std::vector<Submodel> nodes;
		while (!pending.empty())
		{
			if (pending.back().count == 0)
			{
				pending.pop_back();
				continue;
			}

			if (nodes.size() >= BinaryReader::MAX_ELEMENTS || pending.size() > BinaryReader::MAX_DEPTH)
			{
				return std::unexpected("모델 노드 수 또는 계층 깊이가 제한을 초과했습니다.");
			}

			Submodel node{};
			node.parentIndex = pending.back().parentIndex;
			--pending.back().count;

			std::array<f32, 3> position{};
			std::array<f32, 4> rotation{};
			std::array<f32, 3> scale{};
			std::string meshReference;
			std::string materialReference;
			i32 childCount = 0;
			if (!reader.Read(node.name, position, rotation, scale, meshReference, materialReference) ||
				!reader.ReadCount(childCount, sizeof(Submodel)))
			{
				return std::unexpected("모델 노드 데이터가 유효하지 않습니다: " + path.generic_string());
			}

			node.localPosition.Set(position[0], position[1], position[2]);
			node.localRotation.Set(rotation[0], rotation[1], rotation[2], rotation[3]);
			node.localScale.Set(scale[0], scale[1], scale[2]);

			if (meshReference.empty() != materialReference.empty())
			{
				return std::unexpected("모델 노드의 메시와 머티리얼 참조는 함께 지정해야 합니다: " + node.name);
			}

			if (!meshReference.empty() && !materialReference.empty())
			{
				const auto meshPath = ResolveResourcePath(*root, meshReference);
				const auto materialPath = ResolveResourcePath(*root, materialReference);
				if (!meshPath || !materialPath)
				{
					return std::unexpected("모델 리소스 참조 경로가 유효하지 않습니다.");
				}

				node.mesh = assets->LoadAsset<MeshAsset>(*meshPath);
				node.material = assets->LoadAsset<MaterialAsset>(*materialPath);
				if (!node.mesh || !node.material)
				{
					return std::unexpected(std::format("모델 리소스를 로드하지 못했습니다: {}, {}",
						meshPath->generic_string(), materialPath->generic_string()));
				}
			}

			const i32 index = static_cast<i32>(nodes.size());
			nodes.push_back(std::move(node));
			if (childCount > 0)
			{
				pending.push_back({ index, childCount });
			}
		}

		if (!reader.IsAtEnd())
		{
			return std::unexpected("모델 파일 끝에 예상치 못한 데이터가 남아 있습니다: " + path.generic_string());
		}

		return std::make_unique<ModelAsset>(path, std::move(nodes));
	}
}
