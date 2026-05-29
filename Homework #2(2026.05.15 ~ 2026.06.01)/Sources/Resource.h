#pragma once

#include <filesystem>
#include <string>
#include <string_view>

class Resource
{
public:
	Resource() = default;
	virtual ~Resource() = default;

	[[nodiscard]] std::wstring_view GetName() const;
	void SetName(std::wstring_view name_);

	[[nodiscard]] const std::filesystem::path& GetPath() const;
	void SetPath(const std::filesystem::path& path_);

protected:
	std::wstring name;
	std::filesystem::path path;
};

inline std::wstring_view Resource::GetName() const
{
	return name;
}

inline void Resource::SetName(std::wstring_view name_)
{
	name = name_;
}

inline const std::filesystem::path& Resource::GetPath() const
{
	return path;
}

inline void Resource::SetPath(const std::filesystem::path& path_)
{
	path = path_;
}
