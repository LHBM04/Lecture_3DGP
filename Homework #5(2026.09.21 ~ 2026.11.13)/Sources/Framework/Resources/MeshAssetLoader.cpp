#include "Precompiled.hpp"
#include "MeshAssetLoader.hpp"

#include <filesystem>
#include <fstream>
#include <istream>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include <directxtk12/VertexTypes.h>

#include "../Graphics/GPUBuffer.hpp"
#include "../Graphics/GPUDevice.hpp"
#include "../Graphics/GPUMesh.hpp"

#include "../Utilities/BinaryUtility.hpp"

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

	std::expected<std::unique_ptr<MeshAsset>, std::string> MeshAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		if (path.extension() != ".bin")
		{
			return std::unexpected("메시 로더에는 .bin 메시 파일이 필요합니다.");
		}

		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return std::unexpected("메시 파일을 열지 못했습니다: " + path.generic_string());
		}

		u32 sourceVertexCount = 0;
		std::string parsedName;
		const std::streampos beginPosition = file.tellg();
		std::string firstTag;
		if (!ReadString(file, firstTag))
		{
			return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
		}

		if (firstTag == "<Mesh>:" || firstTag == "<SkinnedMesh>:")
		{
			if (!ReadCount(file, sourceVertexCount) || !ReadString(file, parsedName))
			{
				return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
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
				return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
			}

			if (tag == "</Mesh>")
			{
				break;
			}

			if (tag == "<Bounds>:")
			{
				if (!FromBin(file, parsedBoundsCenter) || !FromBin(file, parsedBoundsExtents))
				{
					return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
				}
			}
			else if (tag == "<BoundingBox>:")
			{
				Vector3D<f32> boundsMin{};
				Vector3D<f32> boundsMax{};
				if (!FromBin(file, boundsMin) || !FromBin(file, boundsMax))
				{
					return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
				}
				parsedBoundsCenter = (boundsMin + boundsMax) * 0.5f;
				parsedBoundsExtents = (boundsMax - boundsMin) * 0.5f;
			}
			else if (tag == "<Positions>:")
			{
				u32 count = 0;
				if (!ReadCount(file, count))
				{
					return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
				}

				parsedPositions.resize(count);
				for (Vector3D<f32>& position : parsedPositions)
				{
					if (!FromBin(file, position))
					{
						return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
					}
				}
			}
			else if (tag == "<Colors>:")
			{
				u32 count = 0;
				if (!ReadCount(file, count))
				{
					return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
				}

				parsedColors.resize(count);
				for (ColorRGBA<f32>& color : parsedColors)
				{
					if (!FromBin(file, color))
					{
						return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
					}
				}
			}
			else if (tag == "<Normals>:")
			{
				u32 count = 0;
				if (!ReadCount(file, count))
				{
					return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
				}

				parsedNormals.resize(count);
				for (Vector3D<f32>& normal : parsedNormals)
				{
					if (!FromBin(file, normal))
					{
						return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
					}
				}
			}
			else if (tag == "<TextureCoords>:")
			{
				u32 count = 0;
				if (!ReadCount(file, count))
				{
					return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
				}

				parsedTextureCoords.resize(count);
				for (Vector2D<f32>& uv : parsedTextureCoords)
				{
					if (!FromBin(file, uv))
					{
						return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
					}
				}
			}
			else if (tag == "<SubMeshes>:")
			{
				u32 subMeshCount = 0;
				if (!ReadCount(file, subMeshCount))
				{
					return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
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
					return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
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
						return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
					}
				}
			}
			else if (tag == "<Indices>:")
			{
				u32 count = 0;
				if (!ReadCount(file, count))
				{
					return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
				}

				parsedIndices.resize(count);
				for (u32& index : parsedIndices)
				{
					if (!FromBin(file, index))
					{
						return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
					}
				}
			}
			else
			{
				return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
			}
		}

		if (file.bad() || (!file.eof() && file.peek() != std::char_traits<char>::eof()))
		{
			return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
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
			return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
		}

		if ((sourceVertexCount != 0 && sourceVertexCount != vertexCount)
			|| (!parsedColors.empty() && parsedColors.size() != vertexCount)
			|| (!parsedNormals.empty() && parsedNormals.size() != vertexCount)
			|| (!parsedTextureCoords.empty() && parsedTextureCoords.size() != vertexCount))
		{
			return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
		}

		for (const u32 index : parsedIndices)
		{
			if (index >= vertexCount)
			{
				return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
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

		auto gpuMesh = device.CreateMesh(
			std::as_bytes(std::span<const DirectX::VertexPositionNormalColor>(builtVertices)),
			sizeof(DirectX::VertexPositionNormalColor),
			std::as_bytes(std::span<const u32>(parsedIndices)),
			DXGI_FORMAT_R32_UINT,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		if (!gpuMesh)
		{
			return std::unexpected(gpuMesh.error());
		}

		return std::make_unique<MeshAsset>(
			path,
			MeshAssetData{
				.boundsCenter = parsedBoundsCenter,
				.boundsExtents = parsedBoundsExtents,
				.positions = std::move(parsedPositions),
				.colors = std::move(parsedColors),
				.normals = std::move(parsedNormals),
				.textureCoords = std::move(parsedTextureCoords),
				.vertices = std::move(builtVertices),
				.indices = std::move(parsedIndices),
				.gpuMesh = std::move(*gpuMesh)
			});
	}
}
