#include "Precompiled.h"
#include "Resource.h"

Resource::Resource(
	std::wstring_view resourceName_,
	std::filesystem::path sourcePath_)
	: resourceName(resourceName_), sourcePath(std::move(sourcePath_))
{
}

const std::wstring& Resource::GetName() const noexcept
{
	return resourceName;
}

void Resource::SetName(std::wstring_view resourceName_)
{
	resourceName = resourceName_;
}

const std::filesystem::path& Resource::GetSourcePath() const noexcept
{
	return sourcePath;
}

void Resource::SetSourcePath(std::filesystem::path sourcePath_)
{
	sourcePath = std::move(sourcePath_);
}

const std::filesystem::path& Resource::GetPath() const noexcept
{
	return sourcePath;
}

void Resource::SetPath(std::filesystem::path sourcePath_)
{
	sourcePath = std::move(sourcePath_);
}

bool Resource::IsLoaded() const noexcept
{
	return isLoaded;
}

void Resource::MarkLoaded(bool isLoaded_) noexcept
{
	isLoaded = isLoaded_;
}
