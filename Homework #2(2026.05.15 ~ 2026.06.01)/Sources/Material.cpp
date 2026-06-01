#include "Precompiled.h"

#include "Material.h"

#include "ResourceSystem.h"
#include "Shader.h"

bool Material::Load()
{
	if (path.empty())
	{
		return false;
	}

	std::ifstream file{ path, std::ios::binary };
	if (file.is_open() == false)
	{
		return false;
	}

	if (ReadTag(file, "<Material>:") == false)
	{
		return false;
	}

	if (ReadTag(file, "<BaseColor>:"))
	{
		file.read(reinterpret_cast<char*>(&color), sizeof(ColorRGBA));
	}

	if (ReadTag(file, "<Shader>:"))
	{
		const std::wstring shaderPath{ ReadString(file) };
		shader = ResourceSystem::GetInstance().GetResource<Shader>(shaderPath);
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

const ColorRGBA& Material::GetColor() const
{
	return color;
}

void Material::SetColor(const ColorRGBA& color_)
{
	color = color_;
}

bool Material::ReadTag(std::ifstream& file_, const std::string& expectedTag_)
{
	uint8_t tagLength{ 0 };
	if (static_cast<bool>(file_.read(reinterpret_cast<char*>(&tagLength), sizeof(uint8_t))) == false)
	{
		return false;
	}

	std::string tag(tagLength, '\0');
	if (static_cast<bool>(file_.read(&tag[0], tagLength)) == false)
	{
		return false;
	}

	return tag == expectedTag_;
}

std::wstring Material::ReadString(std::ifstream& file_)
{
	uint8_t strLength{ 0 };
	if (static_cast<bool>(file_.read(reinterpret_cast<char*>(&strLength), sizeof(uint8_t))) == false)
	{
		return L"";
	}

	std::string str(strLength, '\0');
	if (static_cast<bool>(file_.read(&str[0], strLength)) == false)
	{
		return L"";
	}

	return std::wstring(str.begin(), str.end());
}
