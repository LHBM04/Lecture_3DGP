#include "Precompiled.hpp"
#include "MeshAssetLoader.hpp"

#include <filesystem>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "../Graphics/GPUBuffer.hpp"
#include "../Graphics/GPUDevice.hpp"
#include "../Graphics/GPUMesh.hpp"

#include "ResourceImporter.hpp"

namespace TUK::Framework
{
	std::expected<std::unique_ptr<MeshAsset>, std::string> MeshAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		if (path.extension() != ".tukmesh")
		{
			return std::unexpected("메시 로더에는 .tukmesh 파일이 필요합니다.");
		}
		auto metadata = ResourceImporter::Load<MeshMetadata>(path, ResourceType::MESH);
		if (!metadata)
		{
			return std::unexpected(metadata.error());
		}
		if (metadata->positions.empty() || metadata->normals.size() != metadata->positions.size()
			|| (!metadata->uv0.empty() && metadata->uv0.size() != metadata->positions.size())
			|| metadata->indices.empty() || metadata->indices.size() % 3 != 0)
		{
			return std::unexpected("메시 정점 속성 또는 인덱스 개수가 유효하지 않습니다.");
		}
		for (u32 index : metadata->indices)
		{
			if (index >= metadata->positions.size())
			{
				return std::unexpected("메시 인덱스가 정점 범위를 벗어났습니다.");
			}
		}
		std::vector<Vector3D<f32>> parsedPositions;
		std::vector<Vector3D<f32>> parsedNormals;
		std::vector<Vector2D<f32>> parsedUVs;
		parsedPositions.reserve(metadata->positions.size());
		parsedNormals.reserve(metadata->normals.size());
		parsedUVs.reserve(metadata->uv0.size());
		for (const auto& value : metadata->positions)
		{
			parsedPositions.emplace_back(value[0], value[1], value[2]);
		}
		for (const auto& value : metadata->normals)
		{
			parsedNormals.emplace_back(value[0], value[1], value[2]);
		}
		for (const auto& value : metadata->uv0)
		{
			parsedUVs.emplace_back(value[0], value[1]);
		}
		auto parsedIndices = std::move(metadata->indices);

		auto boundsMin = parsedPositions.front();
		auto boundsMax = boundsMin;
		for (const auto& position : parsedPositions)
		{
			boundsMin = Vector3D<f32>::Min(boundsMin, position);
			boundsMax = Vector3D<f32>::Max(boundsMax, position);
		}
		const auto parsedBoundsCenter = boundsMin * 0.5f + boundsMax * 0.5f;
		const auto parsedBoundsExtents = boundsMax * 0.5f - boundsMin * 0.5f;
		const usize vertexCount = parsedPositions.size();

		std::vector<MeshVertex> builtVertices(vertexCount);
		for (usize i = 0; i < vertexCount; ++i)
		{
			const Vector3D<f32>& pos = parsedPositions[i];
			const Vector3D<f32>& norm = parsedNormals[i];

			// Unity UVs start at the lower left. DDS sampling starts at the upper left.
			const DirectX::XMFLOAT2 uv = parsedUVs.empty()
				? DirectX::XMFLOAT2{ 0.0f, 0.0f }
				: DirectX::XMFLOAT2{ parsedUVs[i].GetX(), 1.0f - parsedUVs[i].GetY() };

			builtVertices[i] = MeshVertex{
				DirectX::XMFLOAT3{ pos.GetX(), pos.GetY(), pos.GetZ() },
				DirectX::XMFLOAT3{ norm.GetX(), norm.GetY(), norm.GetZ() },
				DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f },
				uv
			};
		}

		auto gpuMesh = device.CreateMesh(
			std::as_bytes(std::span<const MeshVertex>(builtVertices)),
			sizeof(MeshVertex),
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
				.normals = std::move(parsedNormals),
				.textureCoords = std::move(parsedUVs),
				.vertices = std::move(builtVertices),
				.indices = std::move(parsedIndices),
				.gpuMesh = std::move(*gpuMesh)
			});
	}
}
