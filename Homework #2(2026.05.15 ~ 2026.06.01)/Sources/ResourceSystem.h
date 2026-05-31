#pragma once

#include "Resource.h"
#include "Singleton.h"

class ResourceSystem final : public Singleton<ResourceSystem>
{
	friend class Singleton<ResourceSystem>;

public:
	ResourceSystem() = default;
	~ResourceSystem() override = default;

	void Initialize();
	void Release();

	template <class TResource, class... Args>
	TResource* LoadResource(const std::filesystem::path& path_, Args&&... args_);

	void UnloadResource(const std::filesystem::path& path_);

	template <class TResource>
	[[nodiscard]] TResource* GetResource(std::wstring_view path_);

private:
	static std::wstring NormalizeKey(std::wstring key_);
	static std::wstring NormalizeKey(std::wstring_view key_);

	std::unordered_map<std::wstring, std::unique_ptr<Resource>> resources;
};

template <class TResource>
inline TResource* ResourceSystem::GetResource(std::wstring_view path_)
{
	const std::wstring key{ NormalizeKey(path_) };
	if (std::unordered_map<std::wstring, std::unique_ptr<Resource>>::iterator it{ resources.find(key) }; it != resources.end())
	{
		return static_cast<TResource*>(it->second.get());
	}

	return nullptr;
}

template <class TResource, class... Args>
inline TResource* ResourceSystem::LoadResource(const std::filesystem::path& path_, Args&&... args_)
{
	const std::wstring key{ NormalizeKey(path_.wstring()) };
	if (std::unordered_map<std::wstring, std::unique_ptr<Resource>>::iterator it{ resources.find(key) }; it != resources.end())
	{
		return static_cast<TResource*>(it->second.get());
	}

	std::unique_ptr<TResource> resource{ std::make_unique<TResource>(std::forward<Args>(args_)...) };
	resource->SetPath(path_.wstring());
	resource->SetName(path_.filename().wstring());

	if (resource->Load())
	{
		TResource* ptr{ resource.get() };
		resources[key] = std::move(resource);
		return ptr;
	}

	return nullptr;
}
