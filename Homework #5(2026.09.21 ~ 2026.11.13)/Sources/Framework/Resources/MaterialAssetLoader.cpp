#include "Precompiled.hpp"
#include "MaterialAssetLoader.hpp"

#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <string>
#include <utility>

#include <nlohmann/json.hpp>

#include "../Graphics/GPUDevice.hpp"
#include "../Graphics/GPUTarget.hpp"

#include "../Math/Mathf.hpp"

#include "AssetSubsystem.hpp"
#include "ShaderAsset.hpp"

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

		if (path.extension() != ".json")
		{
			return std::unexpected("머티리얼 로더에는 .json 파일이 필요합니다.");
		}

		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return std::unexpected(std::format("머티리얼 파일을 열지 못했습니다: {}", path.generic_string()));
		}

		const auto parseError = std::unexpected(
			std::format("머티리얼 JSON을 읽지 못했습니다: {}", path.generic_string()));
		const auto json = nlohmann::json::parse(file, nullptr, false);
		if (file.bad() || !json.is_object())
		{
			return parseError;
		}

		ColorRGBA<f32> parsedBaseColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		ColorRGBA<f32> parsedEmissiveColor{ 0.0f, 0.0f, 0.0f, 1.0f };
		ColorRGBA<f32> parsedSpecularColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		const std::pair<const char*, ColorRGBA<f32>&> colorFields[]{
			{ "baseColor", parsedBaseColor },
			{ "emissiveColor", parsedEmissiveColor },
			{ "specularColor", parsedSpecularColor }
		};
		for (const auto& [key, color] : colorFields)
		{
			if (const auto field = json.find(key); field != json.end() && !color.Deserialize(*field))
			{
				return parseError;
			}
		}

		f32 parsedMetallic = 0.0f;
		f32 parsedRoughness = 0.5f;
		const std::pair<const char*, f32&> numberFields[]{
			{ "metallic", parsedMetallic },
			{ "roughness", parsedRoughness }
		};
		for (const auto& [key, number] : numberFields)
		{
			const auto field = json.find(key);
			if (field != json.end() && (!field->is_number() || !Mathf::TryConvertScalar(field->get<f64>(), number)))
			{
				return parseError;
			}
			number = std::clamp(number, 0.0f, 1.0f);
		}

		std::filesystem::path vertexPath;
		std::filesystem::path pixelPath;
		const std::pair<const char*, std::filesystem::path&> shaderFields[]{
			{ "vertexShader", vertexPath },
			{ "pixelShader", pixelPath }
		};
		const auto materialDir = path.parent_path();
		const auto assetRoot = materialDir.parent_path();
		for (const auto& [key, shaderPath] : shaderFields)
		{
			const auto field = json.find(key);
			if (field == json.end() || !field->is_string())
			{
				return parseError;
			}

			const auto& name = field->get_ref<const std::string&>();
			if (name.empty() || name.find('\0') != std::string::npos)
			{
				return parseError;
			}

			std::filesystem::path shader(std::u8string(name.begin(), name.end()));
			if (!shader.is_absolute())
			{
				if (*shader.begin() == "Resources")
				{
					shader = assetRoot / shader.lexically_relative("Resources");
				}
				else if (*shader.begin() == "Shaders")
				{
					shader = assetRoot / shader;
				}
				else
				{
					shader = materialDir / shader;
				}
			}
			shaderPath = shader.lexically_normal();
		}

		const ShaderAsset* vertex = assets->LoadAsset<ShaderAsset>(vertexPath);
		if (!vertex)
		{
			return std::unexpected(std::format("정점 셰이더를 로드하지 못했습니다: {}", vertexPath.generic_string()));
		}

		const ShaderAsset* pixel = assets->LoadAsset<ShaderAsset>(pixelPath);
		if (!pixel)
		{
			return std::unexpected(std::format("픽셀 셰이더를 로드하지 못했습니다: {}", pixelPath.generic_string()));
		}

		// 이걸 고정시켜야 쉐이더를 추가하기 수월해짐
		// 안 그러면 쉐이더 만들 때마다 레이아웃을 추가로 넣어야 함
		static constexpr D3D12_INPUT_ELEMENT_DESC INPUT_ELEMENTS[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC description{};
		description.VS = vertex->GetShader().GetD3DBytecode();
		description.PS = pixel->GetShader().GetD3DBytecode();
		description.InputLayout = { INPUT_ELEMENTS, static_cast<UINT>(std::size(INPUT_ELEMENTS)) };
		description.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		description.NumRenderTargets = 1;
		description.RTVFormats[0] = targetFormat;
		description.SampleMask = UINT_MAX;
		description.SampleDesc = { 1, 0 };
		description.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		description.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		description.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		description.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		description.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		description.DSVFormat = GPUTarget::DEPTH_FORMAT;

		auto pipeline = device.CreateGraphicsPipelineState(description);
		if (!pipeline)
		{
			return std::unexpected(std::format("그래픽스 파이프라인 상태 생성에 실패했습니다: {}", pipeline.error()));
		}

		return std::make_unique<MaterialAsset>(
			path,
			MaterialAssetData{
				.cullMode = D3D12_CULL_MODE_BACK,
				.fillMode = D3D12_FILL_MODE_SOLID,
				.baseColor = parsedBaseColor,
				.emissiveColor = parsedEmissiveColor,
				.specularColor = parsedSpecularColor,
				.metallic = parsedMetallic,
				.roughness = parsedRoughness,
				.vertexShader = vertex,
				.pixelShader = pixel,
				.pipelineState = std::move(*pipeline),
				.targetFormat = targetFormat
			});
	}
}
