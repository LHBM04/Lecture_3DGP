#include "Precompiled.hpp"
#include "MeshAsset.hpp"

#include <istream>
#include <limits>
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

	std::string MeshAsset::Serialize() const
	{
		if (positions.empty() || indices.empty())
		{
			return {};
		}

		std::ostringstream stream(std::ios::binary);
		ToBin(stream, std::string_view("<Mesh>:"));
		ToBin(stream, static_cast<u32>(positions.size()));
		ToBin(stream, GetName().empty() ? std::string_view("Unnamed") : GetName());

		ToBin(stream, std::string_view("<Bounds>:"));
		ToBin(stream, boundsCenter);
		ToBin(stream, boundsExtents);

		ToBin(stream, std::string_view("<Positions>:"));
		ToBin(stream, static_cast<u32>(positions.size()));
		for (const Vector3D<f32>& position : positions)
		{
			ToBin(stream, position);
		}

		ToBin(stream, std::string_view("<Colors>:"));
		ToBin(stream, static_cast<u32>(colors.size()));
		for (const ColorRGBA<f32>& color : colors)
		{
			ToBin(stream, color);
		}

		ToBin(stream, std::string_view("<Normals>:"));
		ToBin(stream, static_cast<u32>(normals.size()));
		for (const Vector3D<f32>& normal : normals)
		{
			ToBin(stream, normal);
		}

		ToBin(stream, std::string_view("<TextureCoords>:"));
		ToBin(stream, static_cast<u32>(textureCoords.size()));
		for (const Vector2D<f32>& uv : textureCoords)
		{
			ToBin(stream, uv);
		}

		ToBin(stream, std::string_view("<Indices>:"));
		ToBin(stream, static_cast<u32>(indices.size()));
		for (const u32 index : indices)
		{
			ToBin(stream, index);
		}

		ToBin(stream, std::string_view("</Mesh>"));
		if (!stream)
		{
			return {};
		}

		return stream.str();
	}

	bool MeshAsset::Deserialize(std::string_view text)
	{
		std::istringstream file(std::string(text), std::ios::binary);
		if (!file || text.empty())
		{
			return false;
		}

		u32 sourceVertexCount = 0;
		std::string parsedName;
		const std::streampos beginPosition = file.tellg();
		std::string firstTag;
		if (!ReadString(file, firstTag))
		{
			return false;
		}

		if (firstTag == "<Mesh>:" || firstTag == "<SkinnedMesh>:")
		{
			if (!ReadCount(file, sourceVertexCount) || !ReadString(file, parsedName))
			{
				return false;
			}
		}
		else
		{
			file.clear();
			file.seekg(beginPosition);
		}

		Vector3D<f32> parsedBoundsCenter{};
		Vector3D<f32> parsedBoundsExtents{};
		std::vector<Vector3D<f32>> parsedPositions;
		std::vector<ColorRGBA<f32>> parsedColors;
		std::vector<Vector3D<f32>> parsedNormals;
		std::vector<Vector2D<f32>> parsedTextureCoords;
		std::vector<std::vector<u32>> parsedSubMeshes;
		std::vector<u32> parsedIndices;

		while (file.peek() != std::char_traits<char>::eof())
		{
			std::string tag;
			if (!ReadString(file, tag))
			{
				return false;
			}

			if (tag == "</Mesh>")
			{
				break;
			}

			if (tag == "<Bounds>:")
			{
				if (!FromBin(file, parsedBoundsCenter) || !FromBin(file, parsedBoundsExtents))
				{
					return false;
				}
			}
			else if (tag == "<BoundingBox>:")
			{
				Vector3D<f32> boundsMin{};
				Vector3D<f32> boundsMax{};
				if (!FromBin(file, boundsMin) || !FromBin(file, boundsMax))
				{
					return false;
				}
				parsedBoundsCenter = (boundsMin + boundsMax) * 0.5f;
				parsedBoundsExtents = (boundsMax - boundsMin) * 0.5f;
			}
			else if (tag == "<Positions>:")
			{
				u32 count = 0;
				if (!ReadCount(file, count))
				{
					return false;
				}

				parsedPositions.resize(count);
				for (Vector3D<f32>& position : parsedPositions)
				{
					if (!FromBin(file, position))
					{
						return false;
					}
				}
			}
			else if (tag == "<Colors>:")
			{
				u32 count = 0;
				if (!ReadCount(file, count))
				{
					return false;
				}

				parsedColors.resize(count);
				for (ColorRGBA<f32>& color : parsedColors)
				{
					if (!FromBin(file, color))
					{
						return false;
					}
				}
			}
			else if (tag == "<Normals>:")
			{
				u32 count = 0;
				if (!ReadCount(file, count))
				{
					return false;
				}

				parsedNormals.resize(count);
				for (Vector3D<f32>& normal : parsedNormals)
				{
					if (!FromBin(file, normal))
					{
						return false;
					}
				}
			}
			else if (tag == "<TextureCoords>:")
			{
				u32 count = 0;
				if (!ReadCount(file, count))
				{
					return false;
				}

				parsedTextureCoords.resize(count);
				for (Vector2D<f32>& uv : parsedTextureCoords)
				{
					if (!FromBin(file, uv))
					{
						return false;
					}
				}
			}
			else if (tag == "<SubMeshes>:")
			{
				u32 subMeshCount = 0;
				if (!ReadCount(file, subMeshCount))
				{
					return false;
				}
				parsedSubMeshes.clear();
				parsedSubMeshes.resize(subMeshCount);
			}
			else if (tag == "<SubMesh>:")
			{
				u32 subMeshIndex = 0;
				u32 indexCount = 0;
				if (!ReadCount(file, subMeshIndex) || !ReadCount(file, indexCount))
				{
					return false;
				}

				if (parsedSubMeshes.size() <= subMeshIndex)
				{
					parsedSubMeshes.resize(static_cast<usize>(subMeshIndex) + 1);
				}

				std::vector<u32>& subMeshIndices = parsedSubMeshes[subMeshIndex];
				subMeshIndices.resize(indexCount);
				for (u32& index : subMeshIndices)
				{
					if (!FromBin(file, index))
					{
						return false;
					}
				}
			}
			else if (tag == "<Indices>:")
			{
				u32 count = 0;
				if (!ReadCount(file, count))
				{
					return false;
				}

				parsedIndices.resize(count);
				for (u32& index : parsedIndices)
				{
					if (!FromBin(file, index))
					{
						return false;
					}
				}
			}
			else
			{
				return false;
			}
		}

		if (file.bad() || (!file.eof() && file.peek() != std::char_traits<char>::eof()))
		{
			return false;
		}

		if (parsedIndices.empty() && !parsedSubMeshes.empty())
		{
			for (const std::vector<u32>& subMeshIndices : parsedSubMeshes)
			{
				parsedIndices.insert(parsedIndices.end(), subMeshIndices.begin(), subMeshIndices.end());
			}
		}

		const usize vertexCount = parsedPositions.size();
		if (vertexCount == 0 || parsedIndices.empty() || parsedIndices.size() % 3 != 0)
		{
			return false;
		}

		if ((sourceVertexCount != 0 && sourceVertexCount != vertexCount)
			|| (!parsedColors.empty() && parsedColors.size() != vertexCount)
			|| (!parsedNormals.empty() && parsedNormals.size() != vertexCount)
			|| (!parsedTextureCoords.empty() && parsedTextureCoords.size() != vertexCount))
		{
			return false;
		}

		for (const u32 index : parsedIndices)
		{
			if (index >= vertexCount)
			{
				return false;
			}
		}

		std::vector<DirectX::VertexPositionNormalColor> builtVertices(vertexCount);
		for (usize i = 0; i < vertexCount; ++i)
		{
			const Vector3D<f32>& pos = parsedPositions[i];
			const Vector3D<f32> norm = i < parsedNormals.size()
				? parsedNormals[i]
				: Vector3D<f32>{ 0.0f, 0.0f, 1.0f };
			const ColorRGBA<f32> col = i < parsedColors.size()
				? parsedColors[i]
				: ColorRGBA<f32>{ 1.0f, 1.0f, 1.0f, 1.0f };

			builtVertices[i] = DirectX::VertexPositionNormalColor{
				DirectX::XMFLOAT3{ pos.GetX(), pos.GetY(), pos.GetZ() },
				DirectX::XMFLOAT3{ norm.GetX(), norm.GetY(), norm.GetZ() },
				DirectX::XMFLOAT4{ col.GetR(), col.GetG(), col.GetB(), col.GetA() }
			};
		}

		SetName(parsedName);
		boundsCenter = parsedBoundsCenter;
		boundsExtents = parsedBoundsExtents;
		positions = std::move(parsedPositions);
		colors = std::move(parsedColors);
		normals = std::move(parsedNormals);
		textureCoords = std::move(parsedTextureCoords);
		vertices = std::move(builtVertices);
		indices = std::move(parsedIndices);
		return true;
	}
}
