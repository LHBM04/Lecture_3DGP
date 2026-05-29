#pragma once

#include <concepts>
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <utility>

#include "Resource.h"
#include "Singleton.h"

class ResourceSystem final : public Singleton<ResourceSystem>
{
public:
	ResourceSystem() = default;
	~ResourceSystem() override = default;

	template <std::derived_from<Resource> TResource, class... Args>
	TResource* GetOrLoadResource(const std::filesystem::path& path_, Args&&... args_);

	template <std::derived_from<Resource> TResource>
	TResource* GetResource(const std::filesystem::path& path_);

	void UnloadResource(const std::filesystem::path& path_);

private:
	std::unordered_map<std::wstring, std::unique_ptr<Resource>> resources;
};

template <std::derived_from<Resource> TResource, class... Args>
inline TResource* ResourceSystem::GetOrLoadResource(const std::filesystem::path& path_, Args&&... args_)
{
	const std::wstring key = path_.wstring();
	if (auto it = resources.find(key); it != resources.end())
	{
		return static_cast<TResource*>(it->second.get());
	}

	auto resource = std::make_unique<TResource>(std::forward<Args>(args_)...);
	resource->SetPath(path_);
	
	TResource* resourcePtr = resource.get();
	resources[key] = std::move(resource);
	
	return resourcePtr;
}

template <std::derived_from<Resource> TResource>
inline TResource* ResourceSystem::GetResource(const std::filesystem::path& path_)
{
	const std::wstring key = path_.wstring();
	if (auto it = resources.find(key); it != resources.end())
	{
		return static_cast<TResource*>(it->second.get());
	}

	return nullptr;
}

inline void ResourceSystem::UnloadResource(const std::filesystem::path& path_)
{
	resources.erase(path_.wstring());
}
