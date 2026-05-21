#include "Precompiled.h"
#include "Material.h"

#include "Shader.h"

bool Material::Load(const std::filesystem::path& path_)
{
	Unload();

	SetPath(path_);
	SetLoaded(true);
	return true;
}

void Material::Unload() noexcept
{
	ClearPath();
	SetLoaded(false);
	albedoColor = ColorRGBA::GetWhite();
	vertexShader = nullptr;
	pixelShader = nullptr;
}

const ColorRGBA& Material::GetAlbedoColor() const noexcept
{
	return albedoColor;
}

void Material::SetAlbedoColor(const ColorRGBA& color_) noexcept
{
	albedoColor = color_;
}

Shader* Material::GetVertexShader() noexcept
{
	return vertexShader;
}

const Shader* Material::GetVertexShader() const noexcept
{
	return vertexShader;
}

void Material::SetVertexShader(Shader* shader_) noexcept
{
	vertexShader = shader_;
}

Shader* Material::GetPixelShader() noexcept
{
	return pixelShader;
}

const Shader* Material::GetPixelShader() const noexcept
{
	return pixelShader;
}

void Material::SetPixelShader(Shader* shader_) noexcept
{
	pixelShader = shader_;
}

const Material& Material::GetDefault() noexcept
{
	static const Material defaultMaterial;
	return defaultMaterial;
}
