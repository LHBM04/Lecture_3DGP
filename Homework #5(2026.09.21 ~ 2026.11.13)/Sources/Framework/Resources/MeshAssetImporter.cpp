#include "Precompiled.hpp"
#include "MeshAssetImporter.hpp"

#include "../Core/BinaryReader.hpp"

#include "../Graphics/GPUBuffer.hpp"
#include "../Graphics/GPUDevice.hpp"
#include "../Graphics/GPUMesh.hpp"

namespace TUK::Framework
{
	std::expected<std::unique_ptr<MeshAsset>, std::string> MeshAssetImporter::Import(
		const std::filesystem::path& path) const
	{
		if (path.extension() != ".tukmesh")
		{
			return std::unexpected("메시 로더에는 .tukmesh 파일이 필요합니다.");
		}

		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return std::unexpected("Cannot open resource: " + path.generic_string());
		}

		BinaryReader reader(file);
		std::vector<Vector3D<f32>> parsedPositions;
		std::vector<Vector3D<f32>> parsedNormals;
		std::vector<Vector2D<f32>> parsedUVs;
		std::vector<u32> parsedIndices;
		i32 count = 0;

		if (!reader.ReadCount(count, sizeof(decltype(parsedPositions)::value_type)))
		{
			return std::unexpected("Invalid mesh attribute count");
		}
		parsedPositions.resize(count);
		for (auto& value : parsedPositions)
		{
			std::array<f32, 3> components{};
			if (!reader.Read(components))
			{
				return std::unexpected("Invalid mesh attribute data");
			}
			value.Set(components[0], components[1], components[2]);
		}

		if (!reader.ReadCount(count, sizeof(decltype(parsedNormals)::value_type)))
		{
			return std::unexpected("Invalid mesh attribute count");
		}
		parsedNormals.resize(count);
		for (auto& value : parsedNormals)
		{
			std::array<f32, 3> components{};
			if (!reader.Read(components))
			{
				return std::unexpected("Invalid mesh attribute data");
			}
			value.Set(components[0], components[1], components[2]);
		}

		if (!reader.ReadCount(count, sizeof(decltype(parsedUVs)::value_type)))
		{
			return std::unexpected("Invalid mesh attribute count");
		}
		parsedUVs.resize(count);
		for (auto& value : parsedUVs)
		{
			std::array<f32, 2> components{};
			if (!reader.Read(components))
			{
				return std::unexpected("Invalid mesh attribute data");
			}
			value.Set(components[0], components[1]);
		}

		if (!reader.ReadCount(count, sizeof(u32)))
		{
			return std::unexpected("Invalid mesh index count");
		}
		parsedIndices.resize(count);
		for (u32& index : parsedIndices)
		{
			if (!reader.Read(index) || index >= parsedPositions.size())
			{
				return std::unexpected("메시 인덱스가 정점 범위를 벗어났습니다.");
			}
		}

		if (!reader.IsAtEnd() || parsedPositions.empty() || parsedNormals.size() != parsedPositions.size()
			|| (!parsedUVs.empty() && parsedUVs.size() != parsedPositions.size())
			|| parsedIndices.empty() || parsedIndices.size() % 3 != 0)
		{
			return std::unexpected("메시 정점 속성 또는 인덱스 개수가 유효하지 않습니다.");
		}

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

		auto mesh = std::make_unique<MeshAsset>(path, std::move(*gpuMesh));
		mesh->SetBoundsCenter(parsedBoundsCenter);
		mesh->SetBoundsExtents(parsedBoundsExtents);
		mesh->SetPositions(std::move(parsedPositions));
		mesh->SetNormals(std::move(parsedNormals));
		mesh->SetTextureCoords(std::move(parsedUVs));
		mesh->SetVertices(std::move(builtVertices));
		mesh->SetIndices(std::move(parsedIndices));

		return mesh;
	}
}
