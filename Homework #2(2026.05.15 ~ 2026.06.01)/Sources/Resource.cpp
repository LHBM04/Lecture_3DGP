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
