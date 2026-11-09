#include "Precompiled.hpp"
#include "MeshAssetLoader.hpp"

#include <filesystem>
#include <fstream>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include <directxtk12/VertexTypes.h>

#include <nlohmann/json.hpp>

#include "../Graphics/GPUBuffer.hpp"
#include "../Graphics/GPUDevice.hpp"
#include "../Graphics/GPUMesh.hpp"

#include "../Math/Mathf.hpp"

namespace TUK::Framework
{
	std::expected<std::unique_ptr<MeshAsset>, std::string> MeshAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		if (path.extension() != ".json")
		{
			return std::unexpected("메시 로더에는 .json 파일이 필요합니다.");
		}

		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return std::unexpected("메시 파일을 열지 못했습니다: " + path.generic_string());
		}

		constexpr u32 MAX_ELEMENT_COUNT = 16 * 1024 * 1024;
		const auto parseError = std::unexpected("메시 JSON을 읽지 못했습니다: " + path.generic_string());
		const auto json = nlohmann::json::parse(file, nullptr, false);
		if (file.bad() || !json.is_object())
		{
			return parseError;
		}

		const auto positions = json.find("positions");
		const auto normals = json.find("normals");
		const auto indices = json.find("indices");
		if (positions == json.end() || !positions->is_array() || positions->empty()
			|| positions->size() > MAX_ELEMENT_COUNT
			|| normals == json.end() || !normals->is_array() || normals->size() != positions->size()
			|| indices == json.end() || !indices->is_array() || indices->empty()
			|| indices->size() > MAX_ELEMENT_COUNT || indices->size() % 3 != 0)
		{
			return parseError;
		}

		std::vector<Vector3D<f32>> parsedPositions;
		std::vector<Vector3D<f32>> parsedNormals;
		const std::pair<const nlohmann::json&, std::vector<Vector3D<f32>>&> vectorFields[]{
			{ *positions, parsedPositions },
			{ *normals, parsedNormals }
		};
		constexpr const char* AXIS_NAMES[]{ "x", "y", "z" };
		for (const auto& [values, outVectors] : vectorFields)
		{
			outVectors.reserve(values.size());
			for (const auto& value : values)
			{
				Vector3D<f32> vector{};
				if (value.is_object())
				{
					for (usize axis = 0; axis < 3; ++axis)
					{
						const auto component = value.find(AXIS_NAMES[axis]);
						if (component == value.end() || !component->is_number()
							|| !Mathf::TryConvertScalar(component->get<f64>(), vector[axis]))
						{
							return parseError;
						}
					}
				}
				else if (!vector.Deserialize(value))
				{
					return parseError;
				}
				outVectors.push_back(vector);
			}
		}

		std::vector<u32> parsedIndices;
		parsedIndices.reserve(indices->size());
		for (const auto& value : *indices)
		{
			if (!value.is_number_integer())
			{
				return parseError;
			}

			const double index = value.get<double>();
			if (index < 0 || index >= static_cast<double>(parsedPositions.size()))
			{
				return parseError;
			}
			parsedIndices.push_back(static_cast<u32>(index));
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

		std::vector<DirectX::VertexPositionNormalColor> builtVertices(vertexCount);
		for (usize i = 0; i < vertexCount; ++i)
		{
			const Vector3D<f32>& pos = parsedPositions[i];
			const Vector3D<f32>& norm = parsedNormals[i];

			builtVertices[i] = DirectX::VertexPositionNormalColor{
				DirectX::XMFLOAT3{ pos.GetX(), pos.GetY(), pos.GetZ() },
				DirectX::XMFLOAT3{ norm.GetX(), norm.GetY(), norm.GetZ() },
				DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f }
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
				.normals = std::move(parsedNormals),
				.vertices = std::move(builtVertices),
				.indices = std::move(parsedIndices),
				.gpuMesh = std::move(*gpuMesh)
			});
	}
}
