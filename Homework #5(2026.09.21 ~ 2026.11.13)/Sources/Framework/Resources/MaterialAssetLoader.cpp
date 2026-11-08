#include "Precompiled.hpp"
#include "MaterialAssetLoader.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "../Graphics/GPUDevice.hpp"
#include "../Graphics/GPUTarget.hpp"

#include "../Utilities/Logger.hpp"

#include "AssetSubsystem.hpp"
#include "ShaderAsset.hpp"

namespace TUK::Framework
{
	MaterialAssetLoader::MaterialAssetLoader(
		AssetSubsystem* assets,
		const GPUDevice& device,
		DXGI_FORMAT targetFormat) noexcept
		: assets(assets)
		, device(device)
		, targetFormat(targetFormat)
	{
		ASSERT_FMT(assets, "머티리얼 로더의 에셋 서브시스템이 nullptr입니다.");
	}

	std::expected<std::unique_ptr<MaterialAsset>, std::string> MaterialAssetLoader::Load(
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
		
		auto material = std::make_unique<MaterialAsset>(path);
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

		const GPUShader* vertexShader = vertex->GetShader();
		const GPUShader* pixelShader = pixel->GetShader();
		if (!vertexShader || !pixelShader)
		{
			return std::unexpected("셰이더 바이트코드가 로드되지 않았습니다.");
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
		description.VS = vertexShader->GetD3DBytecode();
		description.PS = pixelShader->GetD3DBytecode();
		description.InputLayout = { INPUT_ELEMENTS, static_cast<UINT>(std::size(INPUT_ELEMENTS)) };
		description.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		description.NumRenderTargets = 1;
		description.RTVFormats[0] = targetFormat;
		description.SampleMask = UINT_MAX;
		description.SampleDesc = { 1, 0 };
		description.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		description.RasterizerState.FillMode = material->GetFillMode();
		description.RasterizerState.CullMode = material->GetCullMode();
		description.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		description.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		description.DSVFormat = GPUTarget::DEPTH_FORMAT;

		auto pipeline = device.CreateGraphicsPipelineState(description);
		if (!pipeline)
		{
			return std::unexpected(std::format("그래픽스 파이프라인 상태 생성에 실패했습니다: {}", pipeline.error()));
		}

		material->SetGraphicsPipelineState(std::move(*pipeline), targetFormat);
		return material;
	}
}
