#include "Precompiled.h"
#include "Material.h"

bool Material::Load(const std::filesystem::path& path_)
{
	SetPath(path_);
	SetLoaded(true);
	return true;
}

void Material::Unload()
{
	shader = nullptr;
	descriptorTable = {};
	SetLoaded(false);
}

uint64_t Material::GetId() const noexcept
{
	return id;
}

void Material::SetId(uint64_t id_) noexcept
{
	id = id_;
}

const ColorRGBA& Material::GetColor() const noexcept
{
	return color;
}

void Material::SetColor(const ColorRGBA& color_) noexcept
{
	color = color_;
}

Shader* Material::GetShader() noexcept
{
	return shader;
}

const Shader* Material::GetShader() const noexcept
{
	return shader;
}

void Material::SetShader(Shader* shader_) noexcept
{
	shader = shader_;
}

D3D12_GPU_DESCRIPTOR_HANDLE Material::GetDescriptorTable() const noexcept
{
	return descriptorTable;
}

void Material::SetDescriptorTable(D3D12_GPU_DESCRIPTOR_HANDLE descriptorTable_) noexcept
{
	descriptorTable = descriptorTable_;
}
