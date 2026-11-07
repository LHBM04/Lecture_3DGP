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
			return std::unexpected("머티리얼 로더의 에셋 서브시스템이 nullptr입니다.");
		}

		if (path.extension() != ".bin")
		{
			return std::unexpected("머티리얼 로더에는 .bin 파일이 필요합니다.");
		}

		std::ifstream file(path, std::ios::binary);
		if (not file)
		{
			return std::unexpected(std::format("머티리얼 파일을 열지 못했습니다: {}", path.generic_string()));
		}

		const std::string bytes{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
		
		auto material = std::make_unique<MaterialAsset>();
		if (file.bad() || !material->Deserialize(bytes))
		{
			return std::unexpected(std::format("머티리얼 역직렬화에 실패했습니다: {}", path.generic_string()));
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
			return std::unexpected(std::format("정점 셰이더가 미리 로드되지 않았습니다: {}", vertexPath.generic_string()));
		}

		const ShaderAsset* pixel = assets->FindLoadedAsset<ShaderAsset>(pixelPath);
		if (!pixel)
		{
			return std::unexpected(std::format("픽셀 셰이더가 미리 로드되지 않았습니다: {}", pixelPath.generic_string()));
		}

		auto pipeline = GPUGraphicsPipelineState::CreateForMaterial(device, material.get(), vertex, pixel, targetFormat);
		if (!pipeline)
		{
			return std::unexpected(std::format("그래픽스 파이프라인 상태 생성에 실패했습니다: {}", pipeline.error()));
		}

		material->SetGraphicsPipelineState(std::move(*pipeline), targetFormat);
		return material;
	}
}
