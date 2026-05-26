#include "Precompiled.h"
#include "ResourceSystem.h"

bool ResourceSystem::Initialize(const ResourceOptions& options_)
{
	(void)options_;
	return true;
}

void ResourceSystem::Release()
{
	Clear();
}

Resource* ResourceSystem::GetResource(const std::filesystem::path& path_) const
{
	auto iterator{ resources.find(path_) };
	if (resources.end() == iterator)
	{
		return nullptr;
	}

	return iterator->second.get();
}

void ResourceSystem::UnloadResource(const std::filesystem::path& path_)
{
	auto iterator{ resources.find(path_) };
	if (resources.end() == iterator)
	{
		return;
	}

	if (nullptr != iterator->second)
	{
		iterator->second->Unload();
	}

	resources.erase(iterator);
}

void ResourceSystem::Clear()
{
	for (auto& [path, resource] : resources)
	{
		if (nullptr != resource)
		{
			resource->Unload();
		}
	}

	resources.clear();
}
