#include "Precompiled.hpp"
#include "MaterialAsset.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <istream>
#include <iterator>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include "../Graphics/GPUDevice.hpp"
#include "../Graphics/GPUGraphicsPipelineState.hpp"

#include "../Utilities/BinaryUtility.hpp"

#include "ShaderAsset.hpp"

namespace TUK::Framework
{
	namespace
	{
		using BinaryUtility::FromBin;
		using BinaryUtility::ToBin;

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

	std::expected<MaterialAsset, std::string> MaterialAsset::Create(
		const GPUDevice& device,
		const ShaderAsset& vertexShader,
		const ShaderAsset& pixelShader,
		DXGI_FORMAT targetFormat,
		D3D12_CULL_MODE cullMode,
		D3D12_FILL_MODE fillMode)
	{
		static constexpr D3D12_INPUT_ELEMENT_DESC INPUT_ELEMENTS[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineInfo{};
		pipelineInfo.VS = vertexShader.GetShader().GetD3DBytecode();
		pipelineInfo.PS = pixelShader.GetShader().GetD3DBytecode();
		pipelineInfo.InputLayout = { INPUT_ELEMENTS, static_cast<UINT>(std::size(INPUT_ELEMENTS)) };
		pipelineInfo.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineInfo.NumRenderTargets = 1;
		pipelineInfo.RTVFormats[0] = targetFormat;
		pipelineInfo.SampleMask = UINT_MAX;
		pipelineInfo.SampleDesc = { 1, 0 };
		pipelineInfo.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		pipelineInfo.RasterizerState.FillMode = fillMode;
		pipelineInfo.RasterizerState.CullMode = cullMode;
		pipelineInfo.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pipelineInfo.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		pipelineInfo.DepthStencilState.DepthEnable = FALSE;

		auto pipeline = GPUGraphicsPipelineState::Create(device, pipelineInfo);
		if (!pipeline)
		{
			return std::unexpected(pipeline.error());
		}

		MaterialAsset material(std::move(*pipeline), targetFormat, cullMode, fillMode);
		material.vertexShaderPath = vertexShader.GetSourcePath().generic_string();
		material.pixelShaderPath = pixelShader.GetSourcePath().generic_string();
		if (!vertexShader.GetSourcePath().empty())
		{
			std::filesystem::path combined = vertexShader.GetSourcePath();
			if (combined.stem().extension() == ".vert")
			{
				combined = combined.parent_path() / combined.stem().stem();
				combined.replace_extension(".hlsl");
			}
			material.shaderPath = combined.generic_string();
		}
		return material;
	}

	std::string MaterialAsset::Serialize() const
	{
		std::ostringstream stream(std::ios::binary);
		ToBin(stream, std::string_view("<Material>:"));
		ToBin(stream, std::string_view("<AlbedoColor>:"));
		ToBin(stream, albedoColor);
		ToBin(stream, std::string_view("<EmissiveColor>:"));
		ToBin(stream, emissiveColor);
		ToBin(stream, std::string_view("<SpecularColor>:"));
		ToBin(stream, specularColor);
		ToBin(stream, std::string_view("<Glossiness>:"));
		ToBin(stream, glossiness);
		ToBin(stream, std::string_view("<Smoothness>:"));
		ToBin(stream, smoothness);
		ToBin(stream, std::string_view("<Metallic>:"));
		ToBin(stream, metallic);
		ToBin(stream, std::string_view("<SpecularHighlight>:"));
		ToBin(stream, specularHighlight);
		ToBin(stream, std::string_view("<GlossyReflection>:"));
		ToBin(stream, glossyReflection);
		ToBin(stream, std::string_view("<Shader>:"));
		const std::string_view targetShader = shaderPath.empty()
			? std::string_view("Resources/Shaders/Standard.hlsl")
			: std::string_view(shaderPath);
		stream.put('!');
		stream.write(targetShader.data(), static_cast<std::streamsize>(targetShader.size()));
		return stream.str();
	}

	bool MaterialAsset::Deserialize(std::string_view text)
	{
		std::istringstream file(std::string(text), std::ios::binary);
		std::string header;
		if (!ReadString(file, header) || header != "<Material>:")
		{
			return false;
		}

		std::string parsedShaderPath;
		std::string parsedVertexShaderPath;
		std::string parsedPixelShaderPath;
		ColorRGBA<f32> parsedAlbedoColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		ColorRGBA<f32> parsedEmissiveColor{ 0.0f, 0.0f, 0.0f, 1.0f };
		ColorRGBA<f32> parsedSpecularColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		D3D12_CULL_MODE parsedCullMode = D3D12_CULL_MODE_BACK;
		D3D12_FILL_MODE parsedFillMode = D3D12_FILL_MODE_SOLID;
		f32 parsedGlossiness = 0.5f;
		f32 parsedSmoothness = 0.5f;
		f32 parsedMetallic = 0.0f;
		f32 parsedSpecularHighlight = 0.0f;
		f32 parsedGlossyReflection = 0.0f;
		f32 parsedRoughness = 0.5f;
		bool hasGlossiness = false;
		bool hasSmoothness = false;
		bool hasRoughness = false;
		bool hasShaderTag = false;

		while (file.peek() != std::char_traits<char>::eof())
		{
			std::string tag;
			if (!ReadString(file, tag))
			{
				return false;
			}

			if (tag == "</Material>")
			{
				break;
			}

			if (tag == "<Shader>:")
			{
				if (!ReadRemainingShaderPath(file, parsedShaderPath))
				{
					return false;
				}
				hasShaderTag = true;
				break;
			}

			if (tag == "<VertexShader>:" || tag == "<PixelShader>:")
			{
				std::string name;
				if (!ReadString(file, name) || name.empty())
				{
					return false;
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
				if (!FromBin(file, parsedAlbedoColor))
				{
					return false;
				}
			}
			else if (tag == "<EmissiveColor>:")
			{
				if (!FromBin(file, parsedEmissiveColor))
				{
					return false;
				}
			}
			else if (tag == "<SpecularColor>:")
			{
				if (!FromBin(file, parsedSpecularColor))
				{
					return false;
				}
			}
			else if (tag == "<Glossiness>:")
			{
				if (!FromBin(file, parsedGlossiness))
				{
					return false;
				}
				hasGlossiness = true;
			}
			else if (tag == "<Smoothness>:")
			{
				if (!FromBin(file, parsedSmoothness))
				{
					return false;
				}
				hasSmoothness = true;
			}
			else if (tag == "<Metallic>:")
			{
				if (!FromBin(file, parsedMetallic))
				{
					return false;
				}
			}
			else if (tag == "<SpecularHighlight>:")
			{
				if (!FromBin(file, parsedSpecularHighlight))
				{
					return false;
				}
			}
			else if (tag == "<GlossyReflection>:")
			{
				if (!FromBin(file, parsedGlossyReflection))
				{
					return false;
				}
			}
			else if (tag == "<Roughness>:")
			{
				if (!FromBin(file, parsedRoughness))
				{
					return false;
				}
				hasRoughness = true;
			}
			else if (tag == "<CullMode>:" || tag == "<FillMode>:")
			{
				u32 value = 0;
				if (!FromBin(file, value))
				{
					return false;
				}

				if (tag == "<CullMode>:")
				{
					if (value < D3D12_CULL_MODE_NONE || value > D3D12_CULL_MODE_BACK)
					{
						return false;
					}
					parsedCullMode = static_cast<D3D12_CULL_MODE>(value);
				}
				else
				{
					if (value != D3D12_FILL_MODE_WIREFRAME && value != D3D12_FILL_MODE_SOLID)
					{
						return false;
					}
					parsedFillMode = static_cast<D3D12_FILL_MODE>(value);
				}
			}
			else
			{
				return false;
			}
		}

		if (file.bad() || (!file.eof() && file.peek() != std::char_traits<char>::eof()))
		{
			return false;
		}

		parsedMetallic = std::clamp(parsedMetallic, 0.0f, 1.0f);
		parsedGlossiness = std::clamp(parsedGlossiness, 0.0f, 1.0f);
		parsedSmoothness = std::clamp(parsedSmoothness, 0.0f, 1.0f);
		parsedSpecularHighlight = std::clamp(parsedSpecularHighlight, 0.0f, 1.0f);
		parsedGlossyReflection = std::clamp(parsedGlossyReflection, 0.0f, 1.0f);
		parsedRoughness = std::clamp(parsedRoughness, 0.0f, 1.0f);

		if (hasSmoothness)
		{
			parsedRoughness = 1.0f - parsedSmoothness;
		}
		else if (hasRoughness)
		{
			parsedSmoothness = 1.0f - parsedRoughness;
			parsedGlossiness = parsedSmoothness;
		}
		else if (hasGlossiness)
		{
			parsedSmoothness = parsedGlossiness;
			parsedRoughness = 1.0f - parsedSmoothness;
		}
		else
		{
			parsedSmoothness = 1.0f - parsedRoughness;
			parsedGlossiness = parsedSmoothness;
		}

		MaterialAsset candidate;
		candidate.albedoColor = parsedAlbedoColor;
		candidate.emissiveColor = parsedEmissiveColor;
		candidate.specularColor = parsedSpecularColor;
		candidate.glossiness = parsedGlossiness;
		candidate.smoothness = parsedSmoothness;
		candidate.metallic = parsedMetallic;
		candidate.specularHighlight = parsedSpecularHighlight;
		candidate.glossyReflection = parsedGlossyReflection;
		candidate.roughness = parsedRoughness;
		candidate.cullMode = parsedCullMode;
		candidate.fillMode = parsedFillMode;

		if (hasShaderTag && !parsedShaderPath.empty())
		{
			candidate.SetShaderPath(parsedShaderPath);
		}
		else if (!parsedVertexShaderPath.empty() && !parsedPixelShaderPath.empty())
		{
			candidate.vertexShaderPath = std::move(parsedVertexShaderPath);
			candidate.pixelShaderPath = std::move(parsedPixelShaderPath);
		}
		else
		{
			return false;
		}

		*this = std::move(candidate);
		return true;
	}
}
