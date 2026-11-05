#include "Precompiled.hpp"
#include "MaterialAssetLoader.hpp"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "../Utilities/Logger.hpp"

#include "AssetSubsystem.hpp"
#include "ShaderAsset.hpp"

namespace TUK::Framework
{
	MaterialAssetLoader::MaterialAssetLoader(
		const AssetSubsystem* assets,
		const GPUDevice& device,
		DXGI_FORMAT targetFormat) noexcept
		: assets(assets)
		, device(device)
		, targetFormat(targetFormat)
	{
	}

	std::expected<std::unique_ptr<Asset>, std::string> MaterialAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		if (!assets)
		{
			return std::unexpected("AssetSubsystem is null in MaterialAssetLoader.");
		}

		if (path.extension() != ".bin")
		{
			return std::unexpected("MaterialAssetLoader requires a .bin file.");
		}

		std::ifstream file(path, std::ios::binary);
		if (not file)
		{
			return std::unexpected(std::format("Cannot open material: {}", path.generic_string()));
		}

		const std::string bytes{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
		
		auto material = std::make_unique<MaterialAsset>();
		if (file.bad() || !material->Deserialize(bytes))
		{
			return std::unexpected(std::format("Cannot deserialize material: {}", path.generic_string()));
		}

		const auto shaderPath = [&path](std::string_view name)
		{
			const std::filesystem::path shader(std::u8string(name.begin(), name.end()));
			if (shader.is_absolute())
			{
				return shader.lexically_normal();
			}
			if (!shader.empty() && *shader.begin() == "Resources")
			{
				return (path.parent_path().parent_path() / shader.lexically_relative("Resources")).lexically_normal();
			}
			return (path.parent_path() / shader).lexically_normal();
		};

		const auto vertexPath = shaderPath(material->GetVertexShaderPath());
		const auto pixelPath = shaderPath(material->GetPixelShaderPath());
		const ShaderAsset* vertex = assets->FindLoadedAsset<ShaderAsset>(vertexPath);
		if (!vertex)
		{
			return std::unexpected(std::format("Vertex shader was not preloaded: {}", vertexPath.generic_string()));
		}

		const ShaderAsset* pixel = assets->FindLoadedAsset<ShaderAsset>(pixelPath);
		if (!pixel)
		{
			return std::unexpected(std::format("Pixel shader was not preloaded: {}", pixelPath.generic_string()));
		}

		auto pipeline = GPUGraphicsPipelineState::CreateForMaterial(device, material.get(), vertex, pixel, targetFormat);
		if (!pipeline)
		{
			return std::unexpected(std::format("Cannot create graphics pipeline state: {}", pipeline.error()));
		}

		material->SetGraphicsPipelineState(std::move(*pipeline), targetFormat);
		return material;
	}
}
