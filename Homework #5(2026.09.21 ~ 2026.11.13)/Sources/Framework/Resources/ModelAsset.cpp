#include "Precompiled.hpp"
#include "ModelAsset.hpp"
#include "MeshAsset.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <istream>
#include <iterator>
#include <limits>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
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
	namespace
	{
		using BinaryUtility::FromBin;
		using BinaryUtility::ToBin;

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

		[[nodiscard]] std::filesystem::path BuildResourcePath(
			const std::filesystem::path& modelPath, std::string_view directory, const std::string& name)
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

	std::string ModelAsset::Serialize() const
	{
		if (nodes.empty())
		{
			return {};
		}

		std::ostringstream stream(std::ios::binary);
		if (!ToBin(stream, std::string_view("<Model>:")) || !ToBin(stream, name)
			|| !ToBin(stream, std::string_view("<Nodes>:")) || !ToBin(stream, static_cast<u32>(nodes.size())))
		{
			return {};
		}

		for (const auto& node : nodes)
		{
			if (!ToBin(stream, std::string_view("<Node>:"))
				|| !ToBin(stream, node.index)
				|| !ToBin(stream, node.parentIndex)
				|| !ToBin(stream, node.name)
				|| !ToBin(stream, std::string_view("<LocalPosition>:"))
				|| !ToBin(stream, node.localPosition)
				|| !ToBin(stream, std::string_view("<LocalRotation>:"))
				|| !ToBin(stream, node.localRotation)
				|| !ToBin(stream, std::string_view("<LocalScale>:"))
				|| !ToBin(stream, node.localScale)
				|| !ToBin(stream, std::string_view("<Renderers>:"))
				|| !ToBin(stream, static_cast<u32>(node.renderers.size())))
			{
				return {};
			}

			for (const auto& renderer : node.renderers)
			{
				if (!ToBin(stream, std::string_view("<Renderer>:"))
					|| !ToBin(stream, renderer.meshName)
					|| !ToBin(stream, renderer.materialName))
				{
					return {};
				}
			}
		}

		if (!stream.good())
		{
			return {};
		}
		return stream.str();
	}

	bool ModelAsset::Deserialize(std::string_view text)
	{
		std::istringstream file(std::string(text), std::ios::binary);
		std::string modelName;
		u32 nodeCount = 0;
		if (!file || !ReadTag(file, "<Model>:") || !ReadString(file, modelName)
			|| !ReadTag(file, "<Nodes>:") || !ReadCount(file, nodeCount) || nodeCount == 0)
		{
			return false;
		}

		std::vector<Matrix4x4<f32>> nodeMatrices;
		std::vector<ModelNode> parsedNodes;
		std::vector<ModelReference> parsedReferences;
		nodeMatrices.reserve(nodeCount);
		parsedNodes.reserve(nodeCount);

		for (u32 index = 0; index < nodeCount; ++index)
		{
			if (!ReadTag(file, "<Node>:"))
			{
				return false;
			}

			i32 nodeIndex = 0;
			i32 parentIndex = -1;
			std::string nodeName;
			if (!FromBin(file, nodeIndex) || !FromBin(file, parentIndex)
				|| nodeIndex != static_cast<i32>(index) || parentIndex < -1 || parentIndex >= nodeIndex
				|| !ReadString(file, nodeName) || !ReadTag(file, "<LocalPosition>:"))
			{
				return false;
			}

			Vector3D<f32> position{};
			if (!FromBin(file, position) || !ReadTag(file, "<LocalRotation>:"))
			{
				return false;
			}

			Quaternion<f32> rotation{};
			if (!FromBin(file, rotation) || !ReadTag(file, "<LocalScale>:"))
			{
				return false;
			}

			Vector3D<f32> scale{};
			u32 partCount = 0;
			if (!FromBin(file, scale) || !ReadTag(file, "<Renderers>:") || !ReadCount(file, partCount))
			{
				return false;
			}

			Matrix4x4<f32> modelMatrix = Matrix4x4<f32>::TRS(position, rotation, scale);
			if (parentIndex >= 0)
			{
				modelMatrix *= nodeMatrices[static_cast<usize>(parentIndex)];
			}
			nodeMatrices.emplace_back(modelMatrix);

			ModelNode node{};
			node.index = nodeIndex;
			node.parentIndex = parentIndex;
			node.name = std::move(nodeName);
			node.localPosition = position;
			node.localRotation = rotation;
			node.localScale = scale;
			node.renderers.reserve(partCount);

			for (u32 partIndex = 0; partIndex < partCount; ++partIndex)
			{
				std::string meshName;
				std::string materialName;
				if (!ReadTag(file, "<Renderer>:") || !ReadString(file, meshName) || meshName.empty()
					|| !ReadString(file, materialName) || materialName.empty())
				{
					return false;
				}
				node.renderers.push_back(ModelRendererReference{ meshName, materialName });
				parsedReferences.push_back(ModelReference{ std::move(meshName), std::move(materialName), modelMatrix });
			}

			parsedNodes.push_back(std::move(node));
		}

		if (parsedReferences.empty() || file.bad() || file.peek() != std::char_traits<char>::eof())
		{
			return false;
		}

		name = std::move(modelName);
		nodes = std::move(parsedNodes);
		references = std::move(parsedReferences);
		parts.clear();
		return true;
	}

	bool ModelAsset::Upload(const GPUDevice& device, AssetSubsystem& assets, const std::filesystem::path& path)
	{
		std::vector<ModelPart> newParts;
		for (const auto& reference : references)
		{
			std::ifstream file(BuildResourcePath(path, "Meshes", reference.meshName), std::ios::binary);
			if (!file)
			{
				return false;
			}
			const std::string bytes{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
			MeshAsset meshAsset;
			if (file.bad() || !meshAsset.Deserialize(bytes))
			{
				return false;
			}
			const auto* material = assets.LoadAsset<MaterialAsset>(BuildResourcePath(path, "Materials", reference.materialName));
			if (!material)
			{
				return false;
			}
			auto vertices = GPUBuffer::CreateUpload(device, std::as_bytes(meshAsset.GetVertices()));
			auto indices = GPUBuffer::CreateUpload(device, std::as_bytes(meshAsset.GetIndices()));
			if (!vertices || !indices)
			{
				return false;
			}
			auto mesh = GPUMesh::Create(std::move(*vertices), sizeof(DirectX::VertexPositionNormalColor),
				std::move(*indices), DXGI_FORMAT_R32_UINT, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			if (!mesh)
			{
				return false;
			}
			newParts.push_back(ModelPart{ std::move(*mesh), material, reference.localToModel });
		}
		parts = std::move(newParts);
		return true;
	}
}
