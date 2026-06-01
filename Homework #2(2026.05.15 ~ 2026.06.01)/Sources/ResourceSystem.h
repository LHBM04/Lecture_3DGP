#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Logger.h"
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

	[[nodiscard]] static std::wstring NormalizeKey(std::wstring_view path_);

private:
	std::unordered_map<std::wstring, std::unique_ptr<Resource>> resources;
};

template <class TResource>
inline TResource* ResourceSystem::GetResource(std::wstring_view path_)
{
	const std::filesystem::path queryPath{ std::wstring(path_) };
	const std::wstring key{ NormalizeKey(queryPath.lexically_normal().wstring()) };
	if (std::unordered_map<std::wstring, std::unique_ptr<Resource>>::iterator it{ resources.find(key) }; it != resources.end())
	{
		return static_cast<TResource*>(it->second.get());
	}

	std::error_code ec;
	const std::filesystem::path absolute{ std::filesystem::absolute(queryPath, ec).lexically_normal() };
	if (!ec)
	{
		const std::wstring absoluteKey{ NormalizeKey(absolute.wstring()) };
		if (std::unordered_map<std::wstring, std::unique_ptr<Resource>>::iterator it{ resources.find(absoluteKey) }; it != resources.end())
		{
			return static_cast<TResource*>(it->second.get());
		}

		const std::filesystem::path relative{ std::filesystem::relative(absolute, std::filesystem::current_path(), ec).lexically_normal() };
		if (!ec)
		{
			const std::wstring relativeKey{ NormalizeKey(relative.wstring()) };
			if (std::unordered_map<std::wstring, std::unique_ptr<Resource>>::iterator it{ resources.find(relativeKey) }; it != resources.end())
			{
				return static_cast<TResource*>(it->second.get());
			}
		}
	}

	Logger::Trace(L"[Resource] GetResource miss: {}", std::wstring(path_));
	return nullptr;
}

template <class TResource, class... Args>
inline TResource* ResourceSystem::LoadResource(const std::filesystem::path& path_, Args&&... args_)
{
	const std::filesystem::path normalizedPath{ path_.lexically_normal() };
	const std::wstring key{ NormalizeKey(normalizedPath.wstring()) };
	if (std::unordered_map<std::wstring, std::unique_ptr<Resource>>::iterator it{ resources.find(key) }; it != resources.end())
	{
		return static_cast<TResource*>(it->second.get());
	}

	std::unique_ptr<TResource> resource{ std::make_unique<TResource>(std::forward<Args>(args_)...) };
	resource->SetPath(normalizedPath.wstring());
	resource->SetName(path_.filename().wstring());

	if (resource->Load())
	{
		TResource* const ptr{ resource.get() };
		resources[key] = std::move(resource);
		Logger::Trace(L"[Resource] Loaded: {}", normalizedPath.wstring());
		return ptr;
	}

	Logger::Warning(L"[Resource] Load failed: {}", normalizedPath.wstring());
	return nullptr;
}
