#include "Precompiled.hpp"
#include "MaterialAssetLoader.hpp"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "AssetSubsystem.hpp"
#include "ShaderAsset.hpp"

namespace TUK::Framework
{
	namespace
	{
		[[nodiscard]] std::filesystem::path BuildShaderPath(
			const std::filesystem::path& materialPath,
			std::string_view name)
		{
			std::filesystem::path shader(std::u8string(name.begin(), name.end()));
			if (shader.is_absolute())
			{
				return shader;
			}

			if (!shader.empty() && *shader.begin() == "Resources")
			{
				return (materialPath.parent_path().parent_path() / shader.lexically_relative("Resources")).lexically_normal();
			}

			return (materialPath.parent_path() / shader).lexically_normal();
		}
	}

	MaterialAssetLoader::MaterialAssetLoader(
		const GPUDevice& device,
		AssetSubsystem& assets,
		DXGI_FORMAT targetFormat) noexcept
		: device(device)
		, assets(assets)
		, targetFormat(targetFormat)
	{
	}

	std::expected<std::unique_ptr<Asset>, std::string> MaterialAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		if (path.extension() != ".bin")
		{
			return std::unexpected("MaterialAssetLoader requires a .bin file.");
		}

		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return std::unexpected("Cannot open material: " + path.generic_string());
		}

		const std::string bytes{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
		auto material = std::make_unique<MaterialAsset>();
		if (file.bad() || !material->Deserialize(bytes))
		{
			return std::unexpected("Cannot deserialize material: " + path.generic_string());
		}

		const ShaderAsset* vertexShader = assets.LoadAsset<ShaderAsset>(
			BuildShaderPath(path, material->GetVertexShaderPath()));
		const ShaderAsset* pixelShader = assets.LoadAsset<ShaderAsset>(
			BuildShaderPath(path, material->GetPixelShaderPath()));
		if (!vertexShader || !pixelShader)
		{
			return std::unexpected("Cannot load shaders for material: " + path.generic_string());
		}

		auto pipelineMaterial = MaterialAsset::Create(
			device,
			*vertexShader,
			*pixelShader,
			targetFormat,
			material->GetCullMode(),
			material->GetFillMode());
		if (!pipelineMaterial)
		{
			return std::unexpected(pipelineMaterial.error());
		}

		material->SetGraphicsPipelineState(
			std::move(const_cast<GPUGraphicsPipelineState&>(pipelineMaterial->GetGraphicsPipelineState())),
			targetFormat);
		return material;
	}
}
