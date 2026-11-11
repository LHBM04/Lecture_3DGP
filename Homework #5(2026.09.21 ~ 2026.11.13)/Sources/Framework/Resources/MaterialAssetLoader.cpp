#include "Precompiled.hpp"
#include "MaterialAssetLoader.hpp"

#include <algorithm>
#include <filesystem>
#include <format>
#include <memory>
#include <string>
#include <utility>

#include "../Graphics/GPUDevice.hpp"
#include "../Graphics/GPUTarget.hpp"

#include "AssetSubsystem.hpp"
#include "ResourceImporter.hpp"
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

		if (path.extension() != ".tukmaterial")
		{
			return std::unexpected("머티리얼 로더에는 .tukmaterial 파일이 필요합니다.");
		}
		auto metadata = ResourceImporter::Load<MaterialMetadata>(path, ResourceType::MATERIAL);
		if (!metadata)
		{
			return std::unexpected(metadata.error());
		}
		if (metadata->pass > static_cast<u32>(MaterialPass::IMAGE_UI))
		{
			return std::unexpected("지원하지 않는 머티리얼 패스입니다.");
		}
		const auto root = ResourceMetadata::FindRoot(path, "Materials");
		if (!root)
		{
			return std::unexpected(root.error());
		}
		const auto vertexPath = ResourceMetadata::ResolvePath(*root, metadata->vertexShader);
		const auto pixelPath = ResourceMetadata::ResolvePath(*root, metadata->pixelShader);
		if (!vertexPath || !pixelPath)
		{
			return std::unexpected("머티리얼의 셰이더 참조 경로가 유효하지 않습니다.");
		}
		const auto& base = metadata->baseColor;
		const auto& emission = metadata->emissiveColor;
		const auto& specular = metadata->specularColor;
		const ColorRGBA<f32> parsedBaseColor{ base[0], base[1], base[2], base[3] };
		const ColorRGBA<f32> parsedEmissiveColor{ emission[0], emission[1], emission[2], emission[3] };
		const ColorRGBA<f32> parsedSpecularColor{ specular[0], specular[1], specular[2], specular[3] };
		const f32 parsedMetallic = std::clamp(metadata->metallic, 0.0f, 1.0f);
		const f32 parsedRoughness = std::clamp(metadata->roughness, 0.0f, 1.0f);
		const MaterialPass pass = static_cast<MaterialPass>(metadata->pass);
		MaterialTextures textures{};
		for (const auto& binding : metadata->textures)
		{
			if (binding.slot >= textures.size() || textures[binding.slot] || pass != MaterialPass::SURFACE)
			{
				return std::unexpected("머티리얼의 텍스처 슬롯이 유효하지 않습니다.");
			}
			const auto texturePath = ResourceMetadata::ResolvePath(*root, binding.path);
			if (!texturePath)
			{
				return std::unexpected(texturePath.error());
			}
			const TextureAsset* texture = assets->LoadAsset<TextureAsset>(*texturePath);
			if (!texture)
			{
				return std::unexpected("머티리얼 텍스처를 로드하지 못했습니다: " + texturePath->generic_string());
			}
			textures[binding.slot] = texture;
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
			{ &metadata->hullShader, &hull },
			{ &metadata->domainShader, &domain },
			{ &metadata->geometryShader, &geometry }
		};
		for (const auto& [name, target] : extraShaders)
		{
			if (name->empty())
			{
				continue;
			}
			const auto shaderPath = ResourceMetadata::ResolvePath(*root, *name);
			if (!shaderPath || !(*target = assets->LoadAsset<ShaderAsset>(*shaderPath)))
			{
				return std::unexpected("Cannot load material shader: " + *name);
			}
		}
		if (static_cast<bool>(hull) != static_cast<bool>(domain))
		{
			return std::unexpected("Hull and domain shaders must be provided together");
		}

		// 이걸 고정시켜야 쉐이더를 추가하기 수월해짐
		// 안 그러면 쉐이더 만들 때마다 레이아웃을 추가로 넣어야 함
		static constexpr D3D12_INPUT_ELEMENT_DESC INPUT_ELEMENTS[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC description{};
		description.VS = vertex->GetShader().GetD3DBytecode();
		description.PS = pixel->GetShader().GetD3DBytecode();
		if (hull)
		{
			description.HS = hull->GetShader().GetD3DBytecode();
			description.DS = domain->GetShader().GetD3DBytecode();
		}
		if (geometry)
		{
			description.GS = geometry->GetShader().GetD3DBytecode();
		}
		if (pass == MaterialPass::SURFACE)
		{
			description.InputLayout = { INPUT_ELEMENTS, static_cast<UINT>(std::size(INPUT_ELEMENTS)) };
		}
		description.PrimitiveTopologyType = hull ? D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH : D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		description.NumRenderTargets = 1;
		description.RTVFormats[0] = targetFormat;
		description.SampleMask = UINT_MAX;
		description.SampleDesc = { 1, 0 };
		description.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		description.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		description.RasterizerState.CullMode = pass == MaterialPass::SURFACE ? D3D12_CULL_MODE_BACK : D3D12_CULL_MODE_NONE;
		description.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		description.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		description.DSVFormat = GPUTarget::DEPTH_FORMAT;
		if (pass == MaterialPass::SKYBOX)
		{
			description.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
			description.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		}
		else if (pass == MaterialPass::IMAGE_UI)
		{
			description.DepthStencilState.DepthEnable = FALSE;
			description.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

			auto& blend = description.BlendState.RenderTarget[0];
			blend.BlendEnable = TRUE;
			blend.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blend.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			blend.SrcBlendAlpha = D3D12_BLEND_ONE;
			blend.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		}

		auto pipeline = device.CreateGraphicsPipelineState(description);
		if (!pipeline)
		{
			return std::unexpected(std::format("그래픽스 파이프라인 상태 생성에 실패했습니다: {}", pipeline.error()));
		}

		return std::make_unique<MaterialAsset>(
			path,
			MaterialAssetData{
				.cullMode = description.RasterizerState.CullMode,
				.fillMode = D3D12_FILL_MODE_SOLID,
				.baseColor = parsedBaseColor,
				.emissiveColor = parsedEmissiveColor,
				.specularColor = parsedSpecularColor,
				.metallic = parsedMetallic,
				.roughness = parsedRoughness,
				.textures = textures,
				.vertexShader = vertex,
				.pixelShader = pixel,
				.pipelineState = std::move(*pipeline),
				.targetFormat = targetFormat,
				.pass = pass,
				.topology = hull ? D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST : D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
			});
	}
}
