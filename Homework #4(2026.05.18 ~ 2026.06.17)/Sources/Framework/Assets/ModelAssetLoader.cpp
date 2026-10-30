#include "Precompiled.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <cstdint>
#include <fstream>
#include <limits>
#include <sstream>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

#include "ModelAssetLoader.hpp"
#include "ModelAsset.hpp"

#include "Framework/Graphics/GPUDevice.hpp"

namespace
{
	bool ResolveIndex(std::string_view text, std::size_t count, std::size_t& result)
	{
		std::int64_t index = 0;
		const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), index);
		if (text.empty() || error != std::errc{} || end != text.data() + text.size() || index == 0
			|| count > static_cast<std::size_t>(std::numeric_limits<std::int64_t>::max()))
		{
			return false;
		}
		const auto size = static_cast<std::int64_t>(count);
		if (index > size || index < -size)
		{
			return false;
		}
		result = static_cast<std::size_t>(index > 0 ? index - 1 : size + index);
		return true;
	}
}

namespace TUK::Framework
{
	ModelAssetLoader::ModelAssetLoader(const GPUDevice& device) noexcept
		: device(device)
	{
	}

	std::expected<std::unique_ptr<Asset>, std::string> ModelAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		if (path.extension() != ".obj")
		{
			return std::unexpected("ModelAssetLoader requires an .obj file.");
		}
		std::ifstream file(path);
		if (!file)
		{
			return std::unexpected("Failed to open OBJ file.");
		}

		std::vector<std::array<float, 3>> positions;
		std::vector<std::array<float, 2>> texcoords;
		std::vector<std::array<float, 3>> normals;

		std::vector<ModelVertex> vertices;
		std::vector<std::uint32_t> indices;

		std::string line;

		std::size_t lineNumber = 0;
		while (std::getline(file, line))
		{
			++lineNumber;
			std::istringstream stream(line);
			std::string type;
			stream >> type;
			if (type == "v" || type == "vn")
			{
				std::array<float, 3> value{};
				if (!(stream >> value[0] >> value[1] >> value[2]))
				{
					return std::unexpected("Invalid OBJ vector at line " + std::to_string(lineNumber));
				}
				(type == "v" ? positions : normals).push_back(value);
			}
			else if (type == "vt")
			{
				std::array<float, 2> value{};
				if (!(stream >> value[0] >> value[1]))
				{
					return std::unexpected("Invalid OBJ texture coordinate at line " + std::to_string(lineNumber));
				}
				texcoords.push_back(value);
			}
			else if (type == "f")
			{
				std::vector<std::string> corners;
				std::string corner;
				while (stream >> corner && !corner.starts_with('#'))
				{
					corners.push_back(corner);
				}
				if (corners.size() != 3)
				{
					return std::unexpected("OBJ faces must be triangles at line " + std::to_string(lineNumber));
				}
				for (const auto& item : corners)
				{
					const std::string_view token(item);
					const auto first = token.find('/');
					const auto second = first == token.npos ? token.npos : token.find('/', first + 1);
					std::size_t index = 0;
					ModelVertex vertex{};
					if (!ResolveIndex(token.substr(0, first), positions.size(), index))
					{
						return std::unexpected("Invalid OBJ position index at line " + std::to_string(lineNumber));
					}
					std::copy(positions[index].begin(), positions[index].end(), vertex.position);
					if (first != token.npos)
					{
						const auto uv = token.substr(first + 1, second == token.npos ? token.npos : second - first - 1);
						if (!uv.empty())
						{
							if (!ResolveIndex(uv, texcoords.size(), index))
							{
								return std::unexpected("Invalid OBJ texture index at line " + std::to_string(lineNumber));
							}
							std::copy(texcoords[index].begin(), texcoords[index].end(), vertex.texcoord);
						}
						if (second != token.npos)
						{
							if (!ResolveIndex(token.substr(second + 1), normals.size(), index))
							{
								return std::unexpected("Invalid OBJ normal index at line " + std::to_string(lineNumber));
							}
							std::copy(normals[index].begin(), normals[index].end(), vertex.normal);
						}
					}
					if (vertices.size() >= std::numeric_limits<std::uint32_t>::max() / sizeof(ModelVertex))
					{
						return std::unexpected("OBJ mesh is too large.");
					}
					indices.push_back(static_cast<std::uint32_t>(vertices.size()));
					vertices.push_back(vertex);
				}
			}
		}
		if (file.bad() || vertices.empty())
		{
			return std::unexpected("OBJ file could not be read or contains no triangles.");
		}

		const GPUBufferDescription vertexInfo{ .sizeInBytes = vertices.size() * sizeof(ModelVertex), .heapType = D3D12_HEAP_TYPE_UPLOAD };
		auto vertexBuffer = device.CreateGPUBuffer(vertexInfo);
		if (!vertexBuffer)
		{
			return std::unexpected(vertexBuffer.error());
		}
		if (auto result = vertexBuffer->Write(std::as_bytes(std::span(vertices))); !result)
		{
			return std::unexpected(result.error());
		}
		const GPUBufferDescription indexInfo{ .sizeInBytes = indices.size() * sizeof(std::uint32_t), .heapType = D3D12_HEAP_TYPE_UPLOAD };
		auto indexBuffer = device.CreateGPUBuffer(indexInfo);
		if (!indexBuffer)
		{
			return std::unexpected(indexBuffer.error());
		}
		if (auto result = indexBuffer->Write(std::as_bytes(std::span(indices))); !result)
		{
			return std::unexpected(result.error());
		}
		const GPUMeshCreateInfo meshInfo{ .vertexStride = sizeof(ModelVertex) };
		auto mesh = device.CreateGPUMesh(std::move(*vertexBuffer), std::move(*indexBuffer), meshInfo);
		if (!mesh)
		{
			return std::unexpected(mesh.error());
		}
		return std::make_unique<ModelAsset>(std::move(*mesh));
	}
}
