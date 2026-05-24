#include "Precompiled.h"
#include "Resource.h"

const std::filesystem::path& Resource::GetPath() const noexcept
{
	return path;
}

void Resource::SetPath(const std::filesystem::path& path_)
{
	path = path_;
}

bool Resource::IsLoaded() const noexcept
{
	return loaded;
}

void Resource::SetLoaded(bool loaded_) noexcept
{
	loaded = loaded_;
}
