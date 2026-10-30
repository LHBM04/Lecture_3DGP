#include "Precompiled.hpp"

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

#include "Framework/Graphics/D3DDevice.hpp"

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
	ModelAssetLoader::ModelAssetLoader(const D3DDevice& device) noexcept
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
		std::vector<ColorRGB<float>> colors;
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
				if (type == "v")
				{
					positions.push_back(value);
					ColorRGB<float> color(1.0f, 1.0f, 1.0f);
					std::vector<float> extraComponents;
					stream >> std::ws;
					while (stream.peek() != std::char_traits<char>::eof() && stream.peek() != '#')
					{
						float component = 0.0f;
						if (!(stream >> component))
						{
							return std::unexpected("Invalid OBJ vertex component at line " + std::to_string(lineNumber));
						}
						extraComponents.push_back(component);
						stream >> std::ws;
					}
					if (extraComponents.size() == 3 || extraComponents.size() == 4)
					{
						const std::size_t offset = extraComponents.size() - 3;
						color = ColorRGB<float>(extraComponents[offset], extraComponents[offset + 1], extraComponents[offset + 2]);
					}
					else if (extraComponents.size() != 0 && extraComponents.size() != 1)
					{
						return std::unexpected("Invalid OBJ vertex color at line " + std::to_string(lineNumber));
					}
					colors.push_back(color);
				}
				else
				{
					normals.push_back(value);
				}
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
					vertex.position = Vector3D<float>(positions[index][0], positions[index][1], positions[index][2]);
					vertex.color = colors[index];
					if (first != token.npos)
					{
						if (second != token.npos)
						{
							if (!ResolveIndex(token.substr(second + 1), normals.size(), index))
							{
								return std::unexpected("Invalid OBJ normal index at line " + std::to_string(lineNumber));
							}
							vertex.normal = Vector3D<float>(normals[index][0], normals[index][1], normals[index][2]);
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

		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC description{};
		description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		description.Width = vertices.size() * sizeof(ModelVertex);
		description.Height = 1;
		description.DepthOrArraySize = 1;
		description.MipLevels = 1;
		description.SampleDesc.Count = 1;
		description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		auto vertexBuffer = device.CreateD3DBuffer(heapProperties, description, D3D12_RESOURCE_STATE_GENERIC_READ);
		if (!vertexBuffer)
		{
			return std::unexpected(vertexBuffer.error());
		}
		if (auto result = vertexBuffer->Write(std::as_bytes(std::span(vertices))); !result)
		{
			return std::unexpected(result.error());
		}
		description.Width = indices.size() * sizeof(std::uint32_t);
		auto indexBuffer = device.CreateD3DBuffer(heapProperties, description, D3D12_RESOURCE_STATE_GENERIC_READ);
		if (!indexBuffer)
		{
			return std::unexpected(indexBuffer.error());
		}
		if (auto result = indexBuffer->Write(std::as_bytes(std::span(indices))); !result)
		{
			return std::unexpected(result.error());
		}
		auto mesh = device.CreateD3DMesh(std::move(*vertexBuffer), std::move(*indexBuffer), sizeof(ModelVertex), DXGI_FORMAT_R32_UINT, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		if (!mesh)
		{
			return std::unexpected(mesh.error());
		}
		return std::make_unique<ModelAsset>(std::move(*mesh));
	}
}
