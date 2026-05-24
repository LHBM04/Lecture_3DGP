#pragma once

#include "Singleton.h"
#include "Resource.h"

#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <concepts>

class ResourceManager final : public Singleton<ResourceManager>
{
	friend class Singleton<ResourceManager>;

private:
	ResourceManager() = default;
	~ResourceManager() = default;

public:
	template <std::derived_from<Resource> T>
	[[nodiscard]] std::shared_ptr<T> Get(const std::string& name_)
	{
		const std::type_index typeIndex{ typeid(T) };
		auto& typeMap{ resources[typeIndex] };

		const auto it{ typeMap.find(name_) };
		if (it != typeMap.end())
		{
			return std::static_pointer_cast<T>(it->second);
		}

		return nullptr;
	}

	template <std::derived_from<Resource> T, typename... Args>
	std::shared_ptr<T> Create(const std::string& name_, Args&&... args_)
	{
		const std::type_index typeIndex{ typeid(T) };
		auto& typeMap{ resources[typeIndex] };

		if (typeMap.contains(name_))
		{
			return std::static_pointer_cast<T>(typeMap[name_]);
		}

		auto resource{ std::make_shared<T>(std::forward<Args>(args_)...) };
		typeMap[name_] = resource;

		return resource;
	}

	template <std::derived_from<Resource> T>
	void Remove(const std::string& name_)
	{
		const std::type_index typeIndex{ typeid(T) };
		auto it{ resources.find(typeIndex) };
		if (it != resources.end())
		{
			it->second.erase(name_);
		}
	}

	void Clear()
	{
		resources.clear();
	}

private:
	std::unordered_map<std::type_index, std::unordered_map<std::string, std::shared_ptr<Resource>>> resources;
};
