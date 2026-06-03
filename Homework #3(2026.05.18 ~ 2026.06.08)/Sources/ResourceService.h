#pragma once

#include <concepts>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>

#include "IResourceLoader.h"
#include "Resource.h"
#include "Service.h"

class ResourceService final : public Service
{
public:
	template <std::derived_from<Resource> TResource>
	void RegisterLoader(std::unique_ptr<IResourceLoader> loader_);

	template <std::derived_from<Resource> TResource>
	[[nodiscard]] std::unique_ptr<TResource> Load(const std::filesystem::path& path_);

protected:
	void OnAdd() override;
	void OnRemove() override;

private:
	std::unordered_map<std::type_index, std::unique_ptr<IResourceLoader>> loaders;
};

template <std::derived_from<Resource> TResource>
void ResourceService::RegisterLoader(std::unique_ptr<IResourceLoader> loader_)
{
	if (loader_ == nullptr)
	{
		throw std::invalid_argument("Resource loader must not be null.");
	}

	loaders[std::type_index(typeid(TResource))] = std::move(loader_);
}

template <std::derived_from<Resource> TResource>
std::unique_ptr<TResource> ResourceService::Load(const std::filesystem::path& path_)
{
	auto it{ loaders.find(std::type_index(typeid(TResource))) };
	if (it == loaders.end())
	{
		throw std::runtime_error("No resource loader is registered for the requested resource type.");
	}

	std::unique_ptr<Resource> resource{ it->second->Load(path_) };
	TResource* typedResource{ dynamic_cast<TResource*>(resource.get()) };
	if (typedResource == nullptr)
	{
		throw std::runtime_error("Resource loader returned an unexpected resource type.");
	}

	resource.release();
	return std::unique_ptr<TResource>(typedResource);
}
