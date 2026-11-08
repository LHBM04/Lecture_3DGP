#include "Precompiled.hpp"
#include "MaterialAssetLoader.hpp"

#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <istream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include "../Graphics/GPUDevice.hpp"
#include "../Graphics/GPUTarget.hpp"

#include "../Utilities/BinaryUtility.hpp"
#include "../Utilities/Logger.hpp"

#include "AssetSubsystem.hpp"
#include "ShaderAsset.hpp"

namespace TUK::Framework
{
	namespace
	{
		using BinaryUtility::FromBin;

		[[nodiscard]] bool ReadString(std::istream& stream, std::string& value)
		{
			if (!FromBin(stream, value))
			{
				return false;
			}

			return value.find('\0') == std::string::npos;
		}

		[[nodiscard]] bool ReadRemainingShaderPath(std::istream& stream, std::string& value)
		{
			if (stream.peek() == '!')
			{
				stream.get();
				std::ostringstream remaining(std::ios::binary);
				remaining << stream.rdbuf();
				value = remaining.str();
				return !value.empty() && value.find('\0') == std::string::npos;
			}

			return ReadString(stream, value) && !value.empty();
		}
	}

	std::expected<std::unique_ptr<MaterialAsset>, std::string> MaterialAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		AssetSubsystem* assets = GetOwner();
		if (!assets)
		{
			return std::unexpected("머티리얼 로더의 에셋 서브시스템이 nullptr입니다.");
		}

		if (path.extension() != ".bin")
		{
			return std::unexpected("머티리얼 로더에는 .bin 파일이 필요합니다.");
		}

		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return std::unexpected(std::format("머티리얼 파일을 열지 못했습니다: {}", path.generic_string()));
		}

		std::string header;
		if (!ReadString(file, header) || header != "<Material>:")
		{
			return std::unexpected(std::format("머티리얼 역직렬화에 실패했습니다: {}", path.generic_string()));
		}

		std::string parsedShaderPath;
		std::string parsedVertexShaderPath;
		std::string parsedPixelShaderPath;
		ColorRGBA<f32> parsedBaseColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		ColorRGBA<f32> parsedEmissiveColor{ 0.0f, 0.0f, 0.0f, 1.0f };
		ColorRGBA<f32> parsedSpecularColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		D3D12_CULL_MODE parsedCullMode = D3D12_CULL_MODE_BACK;
		D3D12_FILL_MODE parsedFillMode = D3D12_FILL_MODE_SOLID;
		f32 parsedMetallic = 0.0f;
		f32 parsedRoughness = 0.5f;
		bool hasSmoothness = false;
		f32 parsedSmoothness = 0.5f;
		bool hasShaderTag = false;

		while (file.peek() != std::char_traits<char>::eof())
		{
			std::string tag;
			if (!ReadString(file, tag))
			{
				return std::unexpected(std::format("머티리얼 역직렬화에 실패했습니다: {}", path.generic_string()));
			}

			if (tag == "</Material>")
			{
				break;
			}

			if (tag == "<Shader>:")
			{
				if (!ReadRemainingShaderPath(file, parsedShaderPath))
				{
					return std::unexpected(std::format("머티리얼 역직렬화에 실패했습니다: {}", path.generic_string()));
				}
				hasShaderTag = true;
				break;
			}

			if (tag == "<VertexShader>:" || tag == "<PixelShader>:")
			{
				std::string name;
				if (!ReadString(file, name) || name.empty())
				{
					return std::unexpected(std::format("머티리얼 역직렬화에 실패했습니다: {}", path.generic_string()));
				}

				if (tag == "<VertexShader>:")
				{
					parsedVertexShaderPath = std::move(name);
				}
				else
				{
					parsedPixelShaderPath = std::move(name);
				}
			}
			else if (tag == "<AlbedoColor>:" || tag == "<BaseColor>:")
			{
				if (!FromBin(file, parsedBaseColor))
				{
					return std::unexpected(std::format("머티리얼 역직렬화에 실패했습니다: {}", path.generic_string()));
				}
			}
			else if (tag == "<EmissiveColor>:")
			{
				if (!FromBin(file, parsedEmissiveColor))
				{
					return std::unexpected(std::format("머티리얼 역직렬화에 실패했습니다: {}", path.generic_string()));
				}
			}
			else if (tag == "<SpecularColor>:")
			{
				if (!FromBin(file, parsedSpecularColor))
				{
					return std::unexpected(std::format("머티리얼 역직렬화에 실패했습니다: {}", path.generic_string()));
				}
			}
			else if (tag == "<Metallic>:")
			{
				if (!FromBin(file, parsedMetallic))
				{
					return std::unexpected(std::format("머티리얼 역직렬화에 실패했습니다: {}", path.generic_string()));
				}
			}
			else if (tag == "<Roughness>:")
			{
				if (!FromBin(file, parsedRoughness))
				{
					return std::unexpected(std::format("머티리얼 역직렬화에 실패했습니다: {}", path.generic_string()));
				}
			}
			else if (tag == "<Smoothness>:" || tag == "<Glossiness>:")
			{
				if (!FromBin(file, parsedSmoothness))
				{
					return std::unexpected(std::format("머티리얼 역직렬화에 실패했습니다: {}", path.generic_string()));
				}
				hasSmoothness = true;
			}
			else if (tag == "<SpecularHighlight>:" || tag == "<GlossyReflection>:")
			{
				f32 dummy = 0.0f;
				if (!FromBin(file, dummy))
				{
					return std::unexpected(std::format("머티리얼 역직렬화에 실패했습니다: {}", path.generic_string()));
				}
			}
			else if (tag == "<CullMode>:" || tag == "<FillMode>:")
			{
				u32 value = 0;
				if (!FromBin(file, value))
				{
					return std::unexpected(std::format("머티리얼 역직렬화에 실패했습니다: {}", path.generic_string()));
				}

				if (tag == "<CullMode>:")
				{
					if (value < D3D12_CULL_MODE_NONE || value > D3D12_CULL_MODE_BACK)
					{
						return std::unexpected(std::format("머티리얼 역직렬화에 실패했습니다: {}", path.generic_string()));
					}
					parsedCullMode = static_cast<D3D12_CULL_MODE>(value);
				}
				else
				{
					if (value != D3D12_FILL_MODE_WIREFRAME && value != D3D12_FILL_MODE_SOLID)
					{
						return std::unexpected(std::format("머티리얼 역직렬화에 실패했습니다: {}", path.generic_string()));
					}
					parsedFillMode = static_cast<D3D12_FILL_MODE>(value);
				}
			}
			else
			{
				return std::unexpected(std::format("머티리얼 역직렬화에 실패했습니다: {}", path.generic_string()));
			}
		}

		if (file.bad() || (!file.eof() && file.peek() != std::char_traits<char>::eof()))
		{
			return std::unexpected(std::format("머티리얼 역직렬화에 실패했습니다: {}", path.generic_string()));
		}

		parsedMetallic = std::clamp(parsedMetallic, 0.0f, 1.0f);
		if (hasSmoothness)
		{
			parsedRoughness = 1.0f - std::clamp(parsedSmoothness, 0.0f, 1.0f);
		}
		else
		{
			parsedRoughness = std::clamp(parsedRoughness, 0.0f, 1.0f);
		}

		if (hasShaderTag && !parsedShaderPath.empty())
		{
			std::filesystem::path vertex(parsedShaderPath);
			std::filesystem::path pixel(parsedShaderPath);
			vertex.replace_extension(".vert.cso");
			pixel.replace_extension(".frag.cso");
			parsedVertexShaderPath = vertex.generic_string();
			parsedPixelShaderPath = pixel.generic_string();
		}
		else if (parsedVertexShaderPath.empty() || parsedPixelShaderPath.empty())
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

		const auto vertexPath = shaderPath(parsedVertexShaderPath);
		const auto pixelPath = shaderPath(parsedPixelShaderPath);
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
		description.RasterizerState.FillMode = parsedFillMode;
		description.RasterizerState.CullMode = parsedCullMode;
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
				.cullMode = parsedCullMode,
				.fillMode = parsedFillMode,
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
