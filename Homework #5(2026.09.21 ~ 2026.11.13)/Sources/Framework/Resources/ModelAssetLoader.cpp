#include "Precompiled.hpp"
#include "ModelAssetLoader.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <istream>
#include <limits>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "../Graphics/GPUBuffer.hpp"
#include "../Graphics/GPUDevice.hpp"

#include "../Math/Quaternion.hpp"
#include "../Math/Vector3D.hpp"

#include "AssetSubsystem.hpp"
#include "MaterialAsset.hpp"

namespace TUK::Framework
{
	namespace
	{
		template <class TValue>
		[[nodiscard]] bool ReadValue(std::istream& stream, TValue& value)
		{
			return static_cast<bool>(stream.read(reinterpret_cast<char*>(&value), sizeof(value)));
		}

		[[nodiscard]] bool ReadString(std::istream& stream, std::string& value)
		{
			std::uint8_t length = 0;
			if (!ReadValue(stream, length))
			{
				return false;
			}
			value.resize(length);
			return static_cast<bool>(stream.read(value.data(), length)) && value.find('\0') == std::string::npos;
		}

		[[nodiscard]] bool ReadTag(std::istream& stream, std::string_view expected)
		{
			std::string tag;
			return ReadString(stream, tag) && tag == expected;
		}

		[[nodiscard]] bool ReadCount(std::istream& stream, u32& count)
		{
			constexpr u32 MAX_ELEMENT_COUNT = 16 * 1024 * 1024;
			return ReadValue(stream, count) && count <= MAX_ELEMENT_COUNT;
		}

		[[nodiscard]] bool ReadVector3(std::istream& stream, Vector3D<f32>& value)
		{
			return ReadValue(stream, value[0]) && ReadValue(stream, value[1]) && ReadValue(stream, value[2]);
		}

		[[nodiscard]] bool ReadVector3(std::istream& stream, DirectX::XMFLOAT3& value)
		{
			return ReadValue(stream, value.x) && ReadValue(stream, value.y) && ReadValue(stream, value.z);
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

		[[nodiscard]] std::expected<void, std::string> ReadMesh(
			const std::filesystem::path& path, std::vector<DirectX::VertexPositionNormalColor>& vertices, std::vector<u32>& indices)
		{
			std::ifstream file(path, std::ios::binary);
			u32 sourceVertexCount = 0;
			std::string meshName;
			if (!file || !ReadTag(file, "<Mesh>:") || !ReadCount(file, sourceVertexCount)
				|| !ReadString(file, meshName))
			{
				return std::unexpected("메쉬 바이너리 헤더를 읽지 못했습니다: " + path.generic_string());
			}

			bool hasEndTag = false;
			std::string tag;
			while (ReadString(file, tag))
			{
				if (tag == "</Mesh>")
				{
					hasEndTag = true;
					break;
				}
				if (tag == "<Bounds>:" || tag == "<BoundingBox>:")
				{
					Vector3D<f32> first;
					Vector3D<f32> second;
					if (!ReadVector3(file, first) || !ReadVector3(file, second))
					{
						return std::unexpected("메쉬 경계 정보를 읽지 못했습니다.");
					}
					continue;
				}

				u32 count = 0;
				if (!ReadCount(file, count))
				{
					return std::unexpected("메쉬 배열 크기가 유효하지 않습니다.");
				}
				if (tag == "<Positions>:")
				{
					if (count == 0 || count != sourceVertexCount
						|| count > std::numeric_limits<u32>::max() / sizeof(DirectX::VertexPositionNormalColor))
					{
						return std::unexpected("메쉬 정점 수가 유효하지 않습니다.");
					}
					vertices.assign(count, DirectX::VertexPositionNormalColor{
						DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
						DirectX::XMFLOAT3{ 0.0f, 0.0f, 1.0f },
						DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f } });
					for (DirectX::VertexPositionNormalColor& vertex : vertices)
					{
						if (!ReadVector3(file, vertex.position))
						{
							return std::unexpected("메쉬 위치 데이터를 읽지 못했습니다.");
						}
					}
				}
				else if (tag == "<Normals>:" || tag == "<Colors>:")
				{
					if (count != 0 && count != vertices.size())
					{
						return std::unexpected("메쉬 법선 또는 색상 수가 정점 수와 다릅니다.");
					}
					for (u32 index = 0; index < count; ++index)
					{
						Vector3D<f32> value;
						if (!ReadVector3(file, value))
						{
							return std::unexpected("메쉬 법선 또는 색상 데이터를 읽지 못했습니다.");
						}
						if (tag == "<Normals>:")
						{
							vertices[index].normal = DirectX::XMFLOAT3{ value[0], value[1], value[2] };
						}
						else
						{
							f32 alpha = 0.0f;
							if (!ReadValue(file, alpha))
							{
								return std::unexpected("메쉬 색상의 알파를 읽지 못했습니다.");
							}
							vertices[index].color = DirectX::XMFLOAT4{ value[0], value[1], value[2], alpha };
						}
					}
				}
				else if (tag == "<TextureCoords>:")
				{
					file.ignore(static_cast<std::streamsize>(count) * sizeof(f32) * 2);
					if (!file)
					{
						return std::unexpected("메쉬 UV 데이터를 읽지 못했습니다.");
					}
				}
				else if (tag == "<Indices>:")
				{
					if (count == 0 || count % 3 != 0)
					{
						return std::unexpected("메쉬의 삼각형 인덱스 수가 유효하지 않습니다.");
					}
					indices.resize(count);
					for (u32& index : indices)
					{
						if (!ReadValue(file, index) || index >= vertices.size())
						{
							return std::unexpected("메쉬 인덱스가 정점 범위를 벗어났습니다.");
						}
					}
				}
				else
				{
					return std::unexpected("지원하지 않는 메쉬 태그: " + tag);
				}
			}
			if (!hasEndTag || vertices.empty() || indices.empty() || file.bad())
			{
				return std::unexpected("메쉬 바이너리가 완전하지 않습니다.");
			}
			return {};
		}
	}

	ModelAssetLoader::ModelAssetLoader(const GPUDevice& device, AssetSubsystem& assets) noexcept
		: device(device)
		, assets(assets)
	{
	}

	std::expected<std::unique_ptr<Asset>, std::string> ModelAssetLoader::Load(const std::filesystem::path& path) const
	{
		if (path.extension() != ".bin")
		{
			return std::unexpected("ModelAssetLoader requires a .bin model file.");
		}
		std::ifstream file(path, std::ios::binary);
		std::string modelName;
		u32 nodeCount = 0;
		if (!file || !ReadTag(file, "<Model>:") || !ReadString(file, modelName)
			|| !ReadTag(file, "<Nodes>:") || !ReadCount(file, nodeCount) || nodeCount == 0)
		{
			return std::unexpected("모델 바이너리 헤더를 읽지 못했습니다.");
		}

		std::vector<Matrix4x4<f32>> nodeMatrices;
		std::vector<ModelPart> parts;
		nodeMatrices.reserve(nodeCount);
		for (u32 index = 0; index < nodeCount; ++index)
		{
			i32 nodeIndex = -1;
			i32 parentIndex = -1;
			std::string nodeName;
			Vector3D<f32> position;
			Quaternion<f32> rotation;
			Vector3D<f32> scale;
			u32 partCount = 0;
			if (!ReadTag(file, "<Node>:") || !ReadValue(file, nodeIndex) || !ReadValue(file, parentIndex)
				|| nodeIndex != static_cast<i32>(index) || parentIndex < -1 || parentIndex >= nodeIndex
				|| !ReadString(file, nodeName) || !ReadTag(file, "<LocalPosition>:") || !ReadVector3(file, position)
				|| !ReadTag(file, "<LocalRotation>:") || !ReadValue(file, rotation[0]) || !ReadValue(file, rotation[1])
				|| !ReadValue(file, rotation[2]) || !ReadValue(file, rotation[3])
				|| !ReadTag(file, "<LocalScale>:") || !ReadVector3(file, scale)
				|| !ReadTag(file, "<Renderers>:") || !ReadCount(file, partCount))
			{
				return std::unexpected("모델 노드의 계층 또는 변환 정보를 읽지 못했습니다.");
			}
			Matrix4x4<f32> modelMatrix = Matrix4x4<f32>::TRS(position, rotation, scale);
			if (parentIndex >= 0)
			{
				modelMatrix *= nodeMatrices[static_cast<usize>(parentIndex)];
			}
			nodeMatrices.emplace_back(modelMatrix);

			for (u32 partIndex = 0; partIndex < partCount; ++partIndex)
			{
				std::string meshName;
				std::string materialName;
				if (!ReadTag(file, "<Renderer>:") || !ReadString(file, meshName) || meshName.empty()
					|| !ReadString(file, materialName) || materialName.empty())
				{
					return std::unexpected("모델의 메쉬 또는 머터리얼 참조를 읽지 못했습니다.");
				}
				std::vector<DirectX::VertexPositionNormalColor> vertices;
				std::vector<u32> indices;
				if (auto result = ReadMesh(BuildResourcePath(path, "Meshes", meshName), vertices, indices); !result)
				{
					return std::unexpected(result.error());
				}
				auto mesh = CreateMesh(vertices, indices);
				if (!mesh)
				{
					return std::unexpected(mesh.error());
				}
				const MaterialAsset* material = assets.LoadAsset<MaterialAsset>(BuildResourcePath(path, "Materials", materialName));
				if (!material)
				{
					return std::unexpected("모델 머터리얼 로딩에 실패했습니다: " + materialName);
				}
				parts.emplace_back(ModelPart{ std::move(*mesh), material, modelMatrix });
			}
		}
		if (parts.empty() || file.bad() || file.peek() != std::char_traits<char>::eof())
		{
			return std::unexpected("모델 바이너리에 메쉬가 없거나 파일 끝이 유효하지 않습니다.");
		}
		return std::make_unique<ModelAsset>(std::move(modelName), std::move(parts));
	}

	std::expected<GPUMesh, std::string> ModelAssetLoader::CreateMesh(
		std::span<const DirectX::VertexPositionNormalColor> vertices,
		std::span<const u32> indices) const
	{
		auto vertexBuffer = GPUBuffer::CreateUpload(device, std::as_bytes(vertices));
		if (!vertexBuffer)
		{
			return std::unexpected(vertexBuffer.error());
		}

		auto indexBuffer = GPUBuffer::CreateUpload(device, std::as_bytes(indices));
		if (!indexBuffer)
		{
			return std::unexpected(indexBuffer.error());
		}

		return GPUMesh::Create(
			std::move(*vertexBuffer),
			sizeof(DirectX::VertexPositionNormalColor),
			std::move(*indexBuffer),
			DXGI_FORMAT_R32_UINT,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}
