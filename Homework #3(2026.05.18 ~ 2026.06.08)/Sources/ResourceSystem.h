#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Resource.h"
#include "Singleton.h"

class ResourceSystem final : public Singleton<ResourceSystem>
{
public:
	ResourceSystem() = default;
	~ResourceSystem() override = default;

	void Initialize();
	void Release();

	template <class TResource>
	[[nodiscard]] TResource* GetResource(std::wstring_view path_);

	void UnloadResource(const std::filesystem::path& path_);

private:
	template <class TResource, class... Args>
	TResource* LoadResource(const std::filesystem::path& path_, Args&&... args_);

	std::unordered_map<std::wstring, std::unique_ptr<Resource>> resources;
};

template <class TResource>
inline TResource* ResourceSystem::GetResource(std::wstring_view path_)
{
	const std::filesystem::path queryPath{ std::wstring(path_) };
	const std::filesystem::path normalizedPath{ queryPath.lexically_normal() };
	const std::wstring key{ normalizedPath.wstring() };
	if (const auto iter{ resources.find(key) }; iter != resources.end())
	{
		return dynamic_cast<TResource*>(iter->second.get());
	}

	std::error_code errorCode;
	if (std::filesystem::exists(normalizedPath, errorCode) && !errorCode)
	{
		return LoadResource<TResource>(normalizedPath);
	}

	const std::filesystem::path absolutePath{ std::filesystem::absolute(normalizedPath, errorCode).lexically_normal() };
	if (!errorCode)
	{
		const std::wstring absoluteKey{ absolutePath.wstring() };
		if (const auto iter{ resources.find(absoluteKey) }; iter != resources.end())
		{
			return dynamic_cast<TResource*>(iter->second.get());
		}

		if (std::filesystem::exists(absolutePath, errorCode) && !errorCode)
		{
			return LoadResource<TResource>(absolutePath);
		}
	}

	return nullptr;
}

template <class TResource, class... Args>
inline TResource* ResourceSystem::LoadResource(const std::filesystem::path& path_, Args&&... args_)
{
	const std::filesystem::path normalizedPath{ path_.lexically_normal() };
	const std::wstring key{ normalizedPath.wstring() };
	if (const auto iter{ resources.find(key) }; iter != resources.end())
	{
		return dynamic_cast<TResource*>(iter->second.get());
	}

	std::unique_ptr<TResource> resource{ std::make_unique<TResource>(std::forward<Args>(args_)...) };
	resource->SetPath(normalizedPath);
	resource->SetName(normalizedPath.filename().wstring());

	if (!resource->Load())
	{
		return nullptr;
	}

	TResource* const resourcePtr{ resource.get() };
	resources[key] = std::move(resource);
	return resourcePtr;
}
