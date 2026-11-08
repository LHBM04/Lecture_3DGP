#include "Precompiled.hpp"
#include "ModelAssetLoader.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <istream>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "../Utilities/BinaryUtility.hpp"
#include "../Utilities/Logger.hpp"

#include "AssetSubsystem.hpp"
#include "MaterialAsset.hpp"
#include "MeshAsset.hpp"

namespace TUK::Framework
{
	namespace
	{
		using BinaryUtility::FromBin;

		[[nodiscard]] bool ReadString(std::istream& stream, std::string& value)
		{
			if (!FromBin(stream, value))
			{
				return false;
			}
			return value.find('\0') == std::string::npos;
		}

		[[nodiscard]] bool ReadTag(std::istream& stream, std::string_view expected)
		{
			std::string tag;
			return ReadString(stream, tag) && tag == expected;
		}

		[[nodiscard]] bool ReadCount(std::istream& stream, u32& count)
		{
			constexpr u32 MAX_ELEMENT_COUNT = 16 * 1024 * 1024;
			if (!FromBin(stream, count))
			{
				return false;
			}
			return count <= MAX_ELEMENT_COUNT;
		}
	}

	std::expected<std::unique_ptr<ModelAsset>, std::string> ModelAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		AssetSubsystem* assets = GetOwner();
		if (!assets)
		{
			return std::unexpected("모델 로더의 에셋 서브시스템이 nullptr입니다.");
		}

		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return std::unexpected("모델 파일을 열지 못했습니다: " + path.generic_string());
		}

		std::string modelName;
		u32 nodeCount = 0;
		if (!ReadTag(file, "<Model>:") || !ReadString(file, modelName)
			|| !ReadTag(file, "<Nodes>:") || !ReadCount(file, nodeCount) || nodeCount == 0)
		{
			return std::unexpected("모델 역직렬화에 실패했습니다: " + path.generic_string());
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

		std::vector<Submodel> parsedSubmodels;
		parsedSubmodels.reserve(nodeCount);
		bool hasAnyRenderer = false;

		for (u32 index = 0; index < nodeCount; ++index)
		{
			if (!ReadTag(file, "<Node>:"))
			{
				return std::unexpected("모델 역직렬화에 실패했습니다: " + path.generic_string());
			}

			i32 nodeIndex = 0;
			i32 parentIndex = -1;
			std::string nodeName;
			if (!FromBin(file, nodeIndex) || !FromBin(file, parentIndex)
				|| nodeIndex != static_cast<i32>(index) || parentIndex < -1 || parentIndex >= nodeIndex
				|| !ReadString(file, nodeName) || !ReadTag(file, "<LocalPosition>:"))
			{
				return std::unexpected("모델 역직렬화에 실패했습니다: " + path.generic_string());
			}

			Vector3D<f32> position{};
			if (!FromBin(file, position) || !ReadTag(file, "<LocalRotation>:"))
			{
				return std::unexpected("모델 역직렬화에 실패했습니다: " + path.generic_string());
			}

			Quaternion<f32> rotation{};
			if (!FromBin(file, rotation) || !ReadTag(file, "<LocalScale>:"))
			{
				return std::unexpected("모델 역직렬화에 실패했습니다: " + path.generic_string());
			}

			Vector3D<f32> scale{};
			u32 partCount = 0;
			if (!FromBin(file, scale) || !ReadTag(file, "<Renderers>:") || !ReadCount(file, partCount))
			{
				return std::unexpected("모델 역직렬화에 실패했습니다: " + path.generic_string());
			}

			Submodel node{};
			node.parentIndex = parentIndex;
			node.name = std::move(nodeName);
			node.localPosition = position;
			node.localRotation = rotation;
			node.localScale = scale;
			node.renderParts.reserve(partCount);

			for (u32 partIndex = 0; partIndex < partCount; ++partIndex)
			{
				std::string meshName;
				std::string materialName;
				if (!ReadTag(file, "<Renderer>:") || !ReadString(file, meshName) || meshName.empty()
					|| !ReadString(file, materialName) || materialName.empty())
				{
					return std::unexpected("모델 역직렬화에 실패했습니다: " + path.generic_string());
				}

				const auto meshPath = resourcePath("Meshes", meshName);
				const MeshAsset* mesh = assets->LoadAsset<MeshAsset>(meshPath);
				if (!mesh)
				{
					return std::unexpected(std::format("모델의 메시 에셋을 로드하지 못했습니다: {}", meshPath.generic_string()));
				}

				const auto materialPath = resourcePath("Materials", materialName);
				const MaterialAsset* material = assets->LoadAsset<MaterialAsset>(materialPath);
				if (!material)
				{
					return std::unexpected(std::format("모델의 머티리얼 에셋을 로드하지 못했습니다: {}", materialPath.generic_string()));
				}

				node.renderParts.push_back(Submodel::RenderPart{ mesh, material });
				hasAnyRenderer = true;
			}

			parsedSubmodels.push_back(std::move(node));
		}

		if (!hasAnyRenderer || file.bad() || file.peek() != std::char_traits<char>::eof())
		{
			return std::unexpected("모델 역직렬화에 실패했습니다: " + path.generic_string());
		}

		return std::make_unique<ModelAsset>(path, std::move(parsedSubmodels));
	}
}
