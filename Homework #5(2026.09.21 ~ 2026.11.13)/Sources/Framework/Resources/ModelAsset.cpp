#include "Precompiled.hpp"
#include "ModelAsset.hpp"

#include <istream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "../Utilities/BinaryUtility.hpp"

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
	}

	std::string ModelAsset::Serialize() const
	{
		if (submodels.empty())
		{
			return {};
		}

		std::ostringstream stream(std::ios::binary);
		if (!ToBin(stream, std::string_view("<Model>:")) || !ToBin(stream, GetName())
			|| !ToBin(stream, std::string_view("<Nodes>:")) || !ToBin(stream, static_cast<u32>(submodels.size())))
		{
			return {};
		}

		for (usize index = 0; index < submodels.size(); ++index)
		{
			const Submodel& node = submodels[index];
			if (!ToBin(stream, std::string_view("<Node>:"))
				|| !ToBin(stream, static_cast<i32>(index))
				|| !ToBin(stream, node.parentIndex)
				|| !ToBin(stream, node.name)
				|| !ToBin(stream, std::string_view("<LocalPosition>:"))
				|| !ToBin(stream, node.localPosition)
				|| !ToBin(stream, std::string_view("<LocalRotation>:"))
				|| !ToBin(stream, node.localRotation)
				|| !ToBin(stream, std::string_view("<LocalScale>:"))
				|| !ToBin(stream, node.localScale)
				|| !ToBin(stream, std::string_view("<Renderers>:"))
				|| !ToBin(stream, static_cast<u32>(node.renderParts.size())))
			{
				return {};
			}

			for (const Submodel::RenderPart& part : node.renderParts)
			{
				if (!ToBin(stream, std::string_view("<Renderer>:"))
					|| part.meshName.empty() || part.materialName.empty()
					|| !ToBin(stream, part.meshName)
					|| !ToBin(stream, part.materialName))
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

		std::vector<Submodel> parsedSubmodels;
		parsedSubmodels.reserve(nodeCount);
		bool hasAnyRenderer = false;

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
					return false;
				}
				node.renderParts.push_back(Submodel::RenderPart{ std::move(meshName), std::move(materialName) });
				hasAnyRenderer = true;
			}

			parsedSubmodels.push_back(std::move(node));
		}

		if (!hasAnyRenderer || file.bad() || file.peek() != std::char_traits<char>::eof())
		{
			return false;
		}

		SetName(modelName);
		submodels = std::move(parsedSubmodels);
		return true;
	}
}
