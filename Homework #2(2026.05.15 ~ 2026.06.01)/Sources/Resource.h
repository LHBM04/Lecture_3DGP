#pragma once

#include <string>

class Resource
{
public:
	Resource() = default;
	virtual ~Resource() = default;

	Resource(const Resource&) = delete;
	Resource& operator=(const Resource&) = delete;

	virtual bool Load() = 0;
	virtual void Unload() = 0;

	[[nodiscard]] const std::wstring& GetPath() const noexcept;
	void SetPath(std::wstring_view path_);

	[[nodiscard]] const std::wstring& GetName() const noexcept;
	void SetName(std::wstring_view name_);

protected:
	std::wstring path;
	std::wstring name;
};
