#include "Precompiled.h"
#include "Resource.h"

const std::wstring& Resource::GetPath() const noexcept
{
    return path;
}

void Resource::SetPath(std::wstring_view path_)
{
	path = path_;
}

const std::wstring& Resource::GetName() const noexcept
{
    return name;
}

void Resource::SetName(std::wstring_view name_)
{
	name = name_;
}

bool Resource::HasTag(std::ifstream& file_, std::string_view expectedTag_) const
{
	return ReadTag(file_) == expectedTag_;
}

std::string Resource::ReadTag(std::ifstream& file_) const
{
	uint8_t tagLength{ 0 };
	if (static_cast<bool>(file_.read(reinterpret_cast<char*>(&tagLength), sizeof(uint8_t))) == false)
	{
		return "";
	}

	std::string tag(tagLength, '\0');
	if (static_cast<bool>(file_.read(&tag[0], tagLength)) == false)
	{
		return "";
	}

	return tag;
}

std::wstring Resource::ReadString(std::ifstream& file_) const
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

std::wstring Resource::ReadRemainingString(std::ifstream& file_) const
{
	const std::streampos currentPosition{ file_.tellg() };
	if (currentPosition == std::streampos(-1))
	{
		return L"";
	}

	file_.seekg(0, std::ios::end);
	const std::streampos endPosition{ file_.tellg() };
	if (endPosition == std::streampos(-1) || endPosition < currentPosition)
	{
		return L"";
	}

	const std::streamsize remainingSize{ endPosition - currentPosition };
	file_.seekg(currentPosition, std::ios::beg);
	if (remainingSize <= 0)
	{
		return L"";
	}

	std::string str(static_cast<std::size_t>(remainingSize), '\0');
	if (static_cast<bool>(file_.read(str.data(), remainingSize)) == false)
	{
		return L"";
	}

	if (!str.empty() && str.front() == '!')
	{
		str.erase(str.begin());
	}

	return std::wstring(str.begin(), str.end());
}
