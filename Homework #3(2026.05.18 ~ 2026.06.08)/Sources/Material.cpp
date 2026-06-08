#include "Precompiled.h"

#include "Material.h"

#include "Logger.h"
#include "ResourceSystem.h"
#include "Shader.h"

bool Material::Load()
{
	if (path.empty())
	{
		Logger::Critical(L"[Material] Load failed: path is empty.");
		return false;
	}

	std::ifstream file{ path, std::ios::binary };
	if (!file.is_open())
	{
		Logger::Critical(L"[Material] Load failed: cannot open file. path={}", path);
		return false;
	}

	if (!HasTag(file, "<Material>:"))
	{
		Logger::Critical(L"[Material] Load failed: <Material>: tag not found. path={}", path);
		return false;
	}

	bool hasShaderTag{ false };
	bool hasGlossiness{ false };
	bool hasSmoothness{ false };
	bool hasRoughness{ false };
	while (file.peek() != EOF)
	{
		const std::string tag{ ReadTag(file) };
		if (tag.empty())
		{
			break;
		}

		if (tag == "<BaseColor>:" || tag == "<AlbedoColor>:")
		{
			if (!ReadValue(file, albedoColor))
			{
				Logger::Critical(L"[Material] Load failed: cannot read AlbedoColor. path={}", path);
				return false;
			}
		}
		else if (tag == "<EmissiveColor>:")
		{
			if (!ReadValue(file, emissiveColor))
			{
				Logger::Critical(L"[Material] Load failed: cannot read EmissiveColor. path={}", path);
				return false;
			}
		}
		else if (tag == "<SpecularColor>:")
		{
			if (!ReadValue(file, specularColor))
			{
				Logger::Critical(L"[Material] Load failed: cannot read SpecularColor. path={}", path);
				return false;
			}
		}
		else if (tag == "<Glossiness>:")
		{
			if (!ReadValue(file, glossiness))
			{
				Logger::Critical(L"[Material] Load failed: cannot read Glossiness. path={}", path);
				return false;
			}
			hasGlossiness = true;
		}
		else if (tag == "<Smoothness>:")
		{
			if (!ReadValue(file, smoothness))
			{
				Logger::Critical(L"[Material] Load failed: cannot read Smoothness. path={}", path);
				return false;
			}
			hasSmoothness = true;
		}
		else if (tag == "<Metallic>:")
		{
			if (!ReadValue(file, metallic))
			{
				Logger::Critical(L"[Material] Load failed: cannot read Metallic. path={}", path);
				return false;
			}
		}
		else if (tag == "<SpecularHighlight>:")
		{
			if (!ReadValue(file, specularHighlight))
			{
				Logger::Critical(L"[Material] Load failed: cannot read SpecularHighlight. path={}", path);
				return false;
			}
		}
		else if (tag == "<GlossyReflection>:")
		{
			if (!ReadValue(file, glossyReflection))
			{
				Logger::Critical(L"[Material] Load failed: cannot read GlossyReflection. path={}", path);
				return false;
			}
		}
		else if (tag == "<Roughness>:")
		{
			if (!ReadValue(file, roughness))
			{
				Logger::Critical(L"[Material] Load failed: cannot read Roughness. path={}", path);
				return false;
			}
			hasRoughness = true;
		}
		else if (tag == "<Shader>:")
		{
			hasShaderTag = true;

			const std::wstring shaderPath{ ReadRemainingString(file) };
			if (shaderPath.empty())
			{
				Logger::Critical(L"[Material] Load failed: cannot read shader path. material={}", path);
				return false;
			}

			shader = ResourceSystem::GetInstance().GetResource<Shader>(shaderPath);
			if (shader == nullptr)
			{
				Logger::Critical(
					L"[Material] Load failed: shader not found. material={}, shader={}",
					path, shaderPath);
				return false;
			}
		}
		else
		{
			Logger::Warning(
				L"[Material] Unknown tag found. material={}, tag={}",
				path, std::wstring(tag.begin(), tag.end()));
			break;
		}
	}

	if (!hasShaderTag)
	{
		Logger::Critical(L"[Material] Load failed: <Shader>: tag not found. path={}", path);
		return false;
	}

	metallic = std::clamp(metallic, 0.0f, 1.0f);
	glossiness = std::clamp(glossiness, 0.0f, 1.0f);
	smoothness = std::clamp(smoothness, 0.0f, 1.0f);
	specularHighlight = std::clamp(specularHighlight, 0.0f, 1.0f);
	glossyReflection = std::clamp(glossyReflection, 0.0f, 1.0f);
	roughness = std::clamp(roughness, 0.0f, 1.0f);

	if (hasSmoothness)
	{
		roughness = 1.0f - smoothness;
	}
	else if (hasRoughness)
	{
		smoothness = 1.0f - roughness;
	}
	else if (hasGlossiness)
	{
		smoothness = glossiness;
		roughness = 1.0f - smoothness;
	}
	else
	{
		smoothness = 1.0f - roughness;
		glossiness = smoothness;
	}

	return true;
}

void Material::Unload()
{
	shader = nullptr;
}

Shader* Material::GetShader() const
{
	return shader;
}

void Material::SetShader(Shader* shader_)
{
	shader = shader_;
}

const ColorRGBA& Material::GetAlbedoColor() const
{
	return albedoColor;
}

void Material::SetAlbedoColor(const ColorRGBA& color_)
{
	albedoColor = color_;
}

const ColorRGBA& Material::GetBaseColor() const
{
	return albedoColor;
}

void Material::SetBaseColor(const ColorRGBA& color_)
{
	albedoColor = color_;
}

const ColorRGBA& Material::GetEmissiveColor() const
{
	return emissiveColor;
}

void Material::SetEmissiveColor(const ColorRGBA& color_)
{
	emissiveColor = color_;
}

const ColorRGBA& Material::GetSpecularColor() const
{
	return specularColor;
}

void Material::SetSpecularColor(const ColorRGBA& color_)
{
	specularColor = color_;
}

float Material::GetGlossiness() const noexcept
{
	return glossiness;
}

void Material::SetGlossiness(float glossiness_) noexcept
{
	glossiness = std::clamp(glossiness_, 0.0f, 1.0f);
}

float Material::GetSmoothness() const noexcept
{
	return smoothness;
}

void Material::SetSmoothness(float smoothness_) noexcept
{
	smoothness = std::clamp(smoothness_, 0.0f, 1.0f);
	roughness = 1.0f - smoothness;
}

float Material::GetMetallic() const noexcept
{
	return metallic;
}

void Material::SetMetallic(float metallic_) noexcept
{
	metallic = std::clamp(metallic_, 0.0f, 1.0f);
}

float Material::GetSpecularHighlight() const noexcept
{
	return specularHighlight;
}

void Material::SetSpecularHighlight(float specularHighlight_) noexcept
{
	specularHighlight = std::clamp(specularHighlight_, 0.0f, 1.0f);
}

float Material::GetGlossyReflection() const noexcept
{
	return glossyReflection;
}

void Material::SetGlossyReflection(float glossyReflection_) noexcept
{
	glossyReflection = std::clamp(glossyReflection_, 0.0f, 1.0f);
}

float Material::GetRoughness() const noexcept
{
	return roughness;
}

void Material::SetRoughness(float roughness_) noexcept
{
	roughness = std::clamp(roughness_, 0.0f, 1.0f);
	smoothness = 1.0f - roughness;
}
