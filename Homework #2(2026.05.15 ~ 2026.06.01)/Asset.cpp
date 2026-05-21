#include "Precompiled.h"
#include "Asset.h"

const std::filesystem::path& Asset::GetPath() const noexcept
{
	return path;
}

void Asset::SetPath(const std::filesystem::path& path_) noexcept
{
	path = path_;
}

bool Asset::IsLoaded() const noexcept
{
	return isLoaded;
}

void Asset::SetLoaded(bool isLoaded_) noexcept
{
	isLoaded = isLoaded_;
}
