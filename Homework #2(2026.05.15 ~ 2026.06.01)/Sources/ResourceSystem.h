#pragma once

#include "Resource.h"
#include "System.h"

#include <concepts>
#include <filesystem>
#include <memory>
#include <unordered_map>

struct ResourceOptions final
{
};

class ResourceSystem final : public System
{
public:
	using ResourceStorage = std::unordered_map<std::filesystem::path, std::unique_ptr<Resource>>;

	bool Initialize(const ResourceOptions& options_);
	void Release() override;

	template <std::derived_from<Resource> TResource>
	[[nodiscard]] TResource* LoadResource(const std::filesystem::path& path_)
	{
		if (auto iterator{ resources.find(path_) }; resources.end() != iterator)
		{
			return dynamic_cast<TResource*>(iterator->second.get());
		}

		std::unique_ptr<TResource> resource{ std::make_unique<TResource>() };
		TResource* result{ resource.get() };
		if (!result->Load(path_))
		{
			return nullptr;
		}

		resources.emplace(path_, std::move(resource));
		return result;
	}

	[[nodiscard]] Resource* GetResource(const std::filesystem::path& path_) const;

	template <std::derived_from<Resource> TResource>
	[[nodiscard]] TResource* GetResource(const std::filesystem::path& path_) const
	{
		return dynamic_cast<TResource*>(GetResource(path_));
	}

	void UnloadResource(const std::filesystem::path& path_);
	void Clear();

private:
	ResourceStorage resources;
};
