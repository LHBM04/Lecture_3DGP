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

	std::ifstream file(path, std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	if (!ReadTag(file, "<Material>:")) return false;

	if (ReadTag(file, "<BaseColor>:"))
	{
		file.read(reinterpret_cast<char*>(&baseColor.x), sizeof(float));
		file.read(reinterpret_cast<char*>(&baseColor.y), sizeof(float));
		file.read(reinterpret_cast<char*>(&baseColor.z), sizeof(float));
		file.read(reinterpret_cast<char*>(&baseColor.w), sizeof(float));
	}

	if (ReadTag(file, "<Shader>:"))
	{
		std::wstring shaderPath{ ReadString(file) };
		shader = ResourceSystem::GetInstance().GetOrLoadResource<Shader>(shaderPath);
	}

	return true;
}

void Material::Unload()
{
	shader = nullptr;
}

bool Material::ReadTag(std::ifstream& file_, const std::string& expectedTag_)
{
	uint8_t tagLength{ 0 };
	if (!static_cast<bool>(file_.read(reinterpret_cast<char*>(&tagLength), sizeof(uint8_t)))) return false;

	std::string tag(tagLength, '\0');
	if (!static_cast<bool>(file_.read(&tag[0], tagLength))) return false;

	return tag == expectedTag_;
}

std::wstring Material::ReadString(std::ifstream& file_)
{
	uint8_t strLength{ 0 };
	if (!static_cast<bool>(file_.read(reinterpret_cast<char*>(&strLength), sizeof(uint8_t)))) return L"";

	std::string str(strLength, '\0');
	if (!static_cast<bool>(file_.read(&str[0], strLength))) return L"";

	return std::wstring(str.begin(), str.end());
}
