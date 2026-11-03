#include "Precompiled.hpp"
#include "MaterialAssetLoader.hpp"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <istream>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <utility>

#include "AssetSubsystem.hpp"
#include "ShaderAsset.hpp"

namespace TUK::Framework
{
	namespace
	{
		template <class TValue>
		[[nodiscard]] bool ReadValue(std::istream& stream, TValue& value)
		{
			return static_cast<bool>(stream.read(reinterpret_cast<char*>(&value), sizeof(value)));
		}

		[[nodiscard]] std::expected<std::string, std::string> ReadString(std::istream& stream)
		{
			std::uint8_t length = 0;
			if (!ReadValue(stream, length))
			{
				return std::unexpected("머터리얼 문자열 길이를 읽지 못했습니다.");
			}
			std::string value(length, '\0');
			if (!stream.read(value.data(), length) || value.find('\0') != std::string::npos)
			{
				return std::unexpected("머터리얼 문자열이 유효하지 않습니다.");
			}
			return value;
		}

		[[nodiscard]] bool ReadColor(std::istream& stream, ColorRGBA<float>& color)
		{
			float red = 0.0f;
			float green = 0.0f;
			float blue = 0.0f;
			float alpha = 0.0f;
			if (!ReadValue(stream, red) || !ReadValue(stream, green) || !ReadValue(stream, blue) || !ReadValue(stream, alpha))
			{
				return false;
			}
			color = ColorRGBA<float>(red, green, blue, alpha);
			return true;
		}

		[[nodiscard]] std::filesystem::path BuildShaderPath(const std::filesystem::path& materialPath, const std::string& name)
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

	MaterialAssetLoader::MaterialAssetLoader(const GPUDevice& device, AssetSubsystem& assets, DXGI_FORMAT targetFormat) noexcept
		: device(device)
		, assets(assets)
		, targetFormat(targetFormat)
	{
	}

	std::expected<std::unique_ptr<Asset>, std::string> MaterialAssetLoader::Load(const std::filesystem::path& path) const
	{
		if (path.extension() != ".bin")
		{
			return std::unexpected("MaterialAssetLoader requires a .bin file.");
		}
		std::ifstream file(path, std::ios::binary);
		auto header = ReadString(file);
		if (!header || *header != "<Material>:")
		{
			return std::unexpected("머터리얼 파일을 열 수 없거나 <Material>: 태그가 없습니다.");
		}

		std::string vertexShaderName;
		std::string pixelShaderName;
		std::string legacyShaderName;
		ColorRGBA<float> baseColor(1.0f, 1.0f, 1.0f, 1.0f);
		ColorRGBA<float> emissiveColor(0.0f, 0.0f, 0.0f, 1.0f);
		D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK;
		D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_SOLID;
		float metallic = 0.0f;
		float roughness = 0.5f;
		bool hasRoughness = false;
		std::optional<float> smoothness;
		std::optional<float> glossiness;
		bool hasEndTag = false;
		while (file.peek() != std::char_traits<char>::eof())
		{
			auto tag = ReadString(file);
			if (!tag)
			{
				return std::unexpected(tag.error());
			}
			if (*tag == "</Material>")
			{
				hasEndTag = true;
				break;
			}
			if (*tag == "<Shader>:")
			{
				if (file.peek() == '!')
				{
					file.get();
					legacyShaderName.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
				}
				else
				{
					auto name = ReadString(file);
					if (!name)
					{
						return std::unexpected(name.error());
					}
					legacyShaderName = std::move(*name);
				}
				hasEndTag = true;
				break;
			}
			if (*tag == "<VertexShader>:" || *tag == "<PixelShader>:")
			{
				auto name = ReadString(file);
				if (!name || name->empty())
				{
					return std::unexpected("머터리얼 셰이더 경로가 유효하지 않습니다.");
				}
				if (*tag == "<VertexShader>:")
				{
					vertexShaderName = std::move(*name);
				}
				else
				{
					pixelShaderName = std::move(*name);
				}
			}
			else if (*tag == "<AlbedoColor>:" || *tag == "<BaseColor>:")
			{
				if (!ReadColor(file, baseColor))
				{
					return std::unexpected("머터리얼 기본 색상을 읽지 못했습니다.");
				}
			}
			else if (*tag == "<EmissiveColor>:")
			{
				if (!ReadColor(file, emissiveColor))
				{
					return std::unexpected("머터리얼 발광 색상을 읽지 못했습니다.");
				}
			}
			else if (*tag == "<SpecularColor>:")
			{
				ColorRGBA<float> unused;
				if (!ReadColor(file, unused))
				{
					return std::unexpected("머터리얼 반사 색상을 읽지 못했습니다.");
				}
			}
			else if (*tag == "<CullMode>:" || *tag == "<FillMode>:")
			{
				std::uint32_t value = 0;
				if (!ReadValue(file, value))
				{
					return std::unexpected("머터리얼 렌더 설정을 읽지 못했습니다.");
				}
				if (*tag == "<CullMode>:")
				{
					if (value < D3D12_CULL_MODE_NONE || value > D3D12_CULL_MODE_BACK)
					{
						return std::unexpected("CullMode가 유효하지 않습니다.");
					}
					cullMode = static_cast<D3D12_CULL_MODE>(value);
				}
				else
				{
					if (value != D3D12_FILL_MODE_WIREFRAME && value != D3D12_FILL_MODE_SOLID)
					{
						return std::unexpected("FillMode가 유효하지 않습니다.");
					}
					fillMode = static_cast<D3D12_FILL_MODE>(value);
				}
			}
			else if (*tag == "<Metallic>:" || *tag == "<Roughness>:" || *tag == "<Smoothness>:"
				|| *tag == "<Glossiness>:" || *tag == "<SpecularHighlight>:" || *tag == "<GlossyReflection>:")
			{
				float value = 0.0f;
				if (!ReadValue(file, value))
				{
					return std::unexpected("머터리얼 표면 속성을 읽지 못했습니다.");
				}
				if (*tag == "<Metallic>:")
				{
					metallic = value;
				}
				else if (*tag == "<Roughness>:")
				{
					roughness = value;
					hasRoughness = true;
				}
				else if (*tag == "<Smoothness>:")
				{
					smoothness = value;
				}
				else if (*tag == "<Glossiness>:")
				{
					glossiness = value;
				}
			}
			else
			{
				return std::unexpected("지원하지 않는 머터리얼 태그: " + *tag);
			}
		}
		if (!hasEndTag || file.bad() || file.peek() != std::char_traits<char>::eof())
		{
			return std::unexpected("머터리얼 바이너리가 완전하지 않습니다.");
		}

		std::filesystem::path vertexPath;
		std::filesystem::path pixelPath;
		if (!legacyShaderName.empty())
		{
			// 3차 바이너리의 Standard.hlsl 참조를 단계별 셰이더 파일로 해석합니다.
			vertexPath = BuildShaderPath(path, legacyShaderName);
			pixelPath = vertexPath;
			vertexPath.replace_extension(".vert.hlsl");
			pixelPath.replace_extension(".frag.hlsl");
		}
		else
		{
			if (vertexShaderName.empty() || pixelShaderName.empty())
			{
				return std::unexpected("머터리얼에 셰이더 경로가 없습니다.");
			}
			vertexPath = BuildShaderPath(path, vertexShaderName);
			pixelPath = BuildShaderPath(path, pixelShaderName);
		}
		if (vertexPath.extension() != ".hlsl" || vertexPath.stem().extension() != ".vert"
			|| pixelPath.extension() != ".hlsl" || pixelPath.stem().extension() != ".frag")
		{
			return std::unexpected("셰이더 단계 확장자가 유효하지 않습니다.");
		}
		const ShaderAsset* vertexShader = assets.LoadAsset<ShaderAsset>(vertexPath);
		const ShaderAsset* pixelShader = assets.LoadAsset<ShaderAsset>(pixelPath);
		if (!vertexShader || !pixelShader)
		{
			return std::unexpected("머터리얼 셰이더 로딩에 실패했습니다.");
		}
		auto material = MaterialAsset::Create(device, *vertexShader, *pixelShader, targetFormat, cullMode, fillMode);
		if (!material)
		{
			return std::unexpected(material.error());
		}
		material->SetBaseColor(baseColor);
		material->SetEmissiveColor(emissiveColor);
		material->SetMetallic(metallic);
		material->SetRoughness(smoothness ? 1.0f - *smoothness : (hasRoughness ? roughness : (glossiness ? 1.0f - *glossiness : roughness)));
		return std::make_unique<MaterialAsset>(std::move(*material));
	}
}
