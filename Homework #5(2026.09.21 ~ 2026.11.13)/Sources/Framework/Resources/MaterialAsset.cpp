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

#include "../Utilities/BinaryUtility.hpp"

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

	std::string MaterialAsset::Serialize() const
	{
		std::ostringstream stream(std::ios::binary);
		ToBin(stream, std::string_view("<Material>:"));
		ToBin(stream, std::string_view("<BaseColor>:"));
		ToBin(stream, baseColor);
		ToBin(stream, std::string_view("<EmissiveColor>:"));
		ToBin(stream, emissiveColor);
		ToBin(stream, std::string_view("<SpecularColor>:"));
		ToBin(stream, specularColor);
		ToBin(stream, std::string_view("<Metallic>:"));
		ToBin(stream, metallic);
		ToBin(stream, std::string_view("<Roughness>:"));
		ToBin(stream, roughness);
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
				if (!FromBin(file, parsedBaseColor))
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
			else if (tag == "<Metallic>:")
			{
				if (!FromBin(file, parsedMetallic))
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
			}
			else if (tag == "<Smoothness>:" || tag == "<Glossiness>:")
			{
				if (!FromBin(file, parsedSmoothness))
				{
					return false;
				}
				hasSmoothness = true;
			}
			else if (tag == "<SpecularHighlight>:" || tag == "<GlossyReflection>:")
			{
				f32 dummy = 0.0f;
				if (!FromBin(file, dummy))
				{
					return false;
				}
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
		if (hasSmoothness)
		{
			parsedRoughness = 1.0f - std::clamp(parsedSmoothness, 0.0f, 1.0f);
		}
		else
		{
			parsedRoughness = std::clamp(parsedRoughness, 0.0f, 1.0f);
		}

		MaterialAsset candidate;
		candidate.baseColor = parsedBaseColor;
		candidate.emissiveColor = parsedEmissiveColor;
		candidate.specularColor = parsedSpecularColor;
		candidate.metallic = parsedMetallic;
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
