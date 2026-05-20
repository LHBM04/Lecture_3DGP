#include "Precompiled.h"
#include "Asset.h"

const std::filesystem::path& Asset::GetPath() const noexcept
{
	return path;
}

bool Asset::IsLoaded() const noexcept
{
	return isLoaded;
}

void Asset::SetPath(const std::filesystem::path& path_)
{
	path = path_;
}

void Asset::ClearPath() noexcept
{
	path.clear();
}

void Asset::SetLoaded(bool isLoaded_) noexcept
{
	isLoaded = isLoaded_;
}
