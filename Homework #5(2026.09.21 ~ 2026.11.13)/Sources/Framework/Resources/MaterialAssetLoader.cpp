#include "Precompiled.hpp"
#include "MaterialAssetLoader.hpp"

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <format>
#include <memory>
#include <string>
#include <utility>

#include "../Core/BinaryReader.hpp"

#include "../Graphics/GPUDevice.hpp"

#include "AssetSubsystem.hpp"
#include "ResourcePath.hpp"
#include "ShaderAsset.hpp"
#include "TextureAsset.hpp"

namespace TUK::Framework
{
	std::expected<std::unique_ptr<MaterialAsset>, std::string> MaterialAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		AssetSubsystem* assets = GetOwner();
		if (!assets)
		{
			return std::unexpected("머티리얼 로더의 에셋 서브시스템이 nullptr입니다.");
		}

		if (path.extension() != ".tukmat")
		{
			return std::unexpected("머티리얼 로더에는 .tukmat 파일이 필요합니다.");
		}
		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return std::unexpected("Cannot open resource: " + path.generic_string());
		}
		BinaryReader reader(file);
		std::array<f32, 4> base{};
		std::array<f32, 4> emission{};
		f32 metallic = 0.0f;
		f32 roughness = 0.5f;
		std::array<std::string, 9> textureReferences;
		std::string vertexShader;
		std::string hullShader;
		std::string domainShader;
		std::string geometryShader;
		std::string pixelShader;
		if (!reader.Read(base, emission, metallic, roughness, textureReferences,
			vertexShader, hullShader, domainShader, geometryShader, pixelShader) || !reader.IsAtEnd())
		{
			return std::unexpected("Invalid material data: " + path.generic_string());
		}
		const auto root = FindResourceRoot(path, "Materials");
		if (!root)
		{
			return std::unexpected(root.error());
		}
		const auto vertexPath = ResolveResourcePath(*root, vertexShader);
		const auto pixelPath = ResolveResourcePath(*root, pixelShader);
		if (!vertexPath || !pixelPath)
		{
			return std::unexpected("머티리얼의 셰이더 참조 경로가 유효하지 않습니다.");
		}
		const ColorRGBA<f32> parsedBaseColor{ base[0], base[1], base[2], base[3] };
		const ColorRGBA<f32> parsedEmissiveColor{ emission[0], emission[1], emission[2], emission[3] };
		const ColorRGBA<f32> parsedSpecularColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		const f32 parsedMetallic = std::clamp(metallic, 0.0f, 1.0f);
		const f32 parsedRoughness = std::clamp(roughness, 0.0f, 1.0f);
		MaterialTextures textures{};
		// Height is stored at file slot 6, but has no runtime texture slot.
		constexpr usize FILE_SLOTS[]{ 0, 1, 2, 3, 4, 5, 7, 8 };
		for (usize slot = 0; slot < textures.size(); ++slot)
		{
			const auto& reference = textureReferences[FILE_SLOTS[slot]];
			if (reference.empty())
			{
				continue;
			}
			const auto texturePath = ResolveResourcePath(*root, reference);
			if (!texturePath)
			{
				return std::unexpected(texturePath.error());
			}
			const TextureAsset* texture = assets->LoadAsset<TextureAsset>(*texturePath);
			if (!texture)
			{
				return std::unexpected("Cannot load material texture: " + texturePath->generic_string());
			}
			textures[slot] = texture;
		}

		const ShaderAsset* vertex = assets->LoadAsset<ShaderAsset>(*vertexPath);
		if (!vertex)
		{
			return std::unexpected(std::format("정점 셰이더를 로드하지 못했습니다: {}", vertexPath->generic_string()));
		}

		const ShaderAsset* pixel = assets->LoadAsset<ShaderAsset>(*pixelPath);
		if (!pixel)
		{
			return std::unexpected(std::format("픽셀 셰이더를 로드하지 못했습니다: {}", pixelPath->generic_string()));
		}

		const ShaderAsset* hull = nullptr;
		const ShaderAsset* domain = nullptr;
		const ShaderAsset* geometry = nullptr;
		const std::pair<const std::string*, const ShaderAsset**> extraShaders[]
		{
			{ &hullShader, &hull },
			{ &domainShader, &domain },
			{ &geometryShader, &geometry }
		};
		for (const auto& [name, target] : extraShaders)
		{
			if (name->empty())
			{
				continue;
			}
			const auto shaderPath = ResolveResourcePath(*root, *name);
			if (!shaderPath || !(*target = assets->LoadAsset<ShaderAsset>(*shaderPath)))
			{
				return std::unexpected("Cannot load material shader: " + *name);
			}
		}
		if (static_cast<bool>(hull) != static_cast<bool>(domain))
		{
			return std::unexpected("Hull and domain shaders must be provided together");
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC description = vertex->CreatePipelineDescription(targetFormat);
		description.VS = vertex->GetShader().GetD3DBytecode();
		description.PS = pixel->GetShader().GetD3DBytecode();
		description.HS = hull ? hull->GetShader().GetD3DBytecode() : D3D12_SHADER_BYTECODE{};
		description.DS = domain ? domain->GetShader().GetD3DBytecode() : D3D12_SHADER_BYTECODE{};
		description.GS = geometry ? geometry->GetShader().GetD3DBytecode() : D3D12_SHADER_BYTECODE{};

		D3D_PRIMITIVE_TOPOLOGY primitive = vertex->GetPrimitive();
		if (hull)
		{
			description.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
			primitive = D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
		}

		auto pipeline = device.CreateGraphicsPipelineState(description);
		if (!pipeline)
		{
			return std::unexpected(std::format("그래픽스 파이프라인 상태 생성에 실패했습니다: {}", pipeline.error()));
		}

		auto material = std::make_unique<MaterialAsset>(path, vertex, pixel, std::move(*pipeline));
		material->SetCullMode(description.RasterizerState.CullMode);
		material->SetFillMode(description.RasterizerState.FillMode);
		material->SetBaseColor(parsedBaseColor);
		material->SetEmissiveColor(parsedEmissiveColor);
		material->SetSpecularColor(parsedSpecularColor);
		material->SetMetallic(parsedMetallic);
		material->SetRoughness(parsedRoughness);
		material->SetTextures(textures);
		material->SetTargetFormat(description.RTVFormats[0]);
		material->SetPrimitive(primitive);

		return material;
	}
}
