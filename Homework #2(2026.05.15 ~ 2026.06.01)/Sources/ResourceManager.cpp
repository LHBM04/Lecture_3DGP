#include "Precompiled.h"
#include "ResourceManager.h"

namespace
{
	ResourceManager::ResourceStorage resources;
}

ResourceManager::ResourceStorage& ResourceManager::GetStorage() noexcept
{
	return resources;
}

Resource* ResourceManager::GetResource(const std::filesystem::path& path_)
{
	auto iterator{ resources.find(path_) };
	if (resources.end() == iterator)
	{
		return nullptr;
	}

	return iterator->second.get();
}

void ResourceManager::UnloadResource(const std::filesystem::path& path_)
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

void ResourceManager::Clear()
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
