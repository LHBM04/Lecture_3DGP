#pragma once

#include <concepts>
#include <expected>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

#include "SystemBase.h"

struct EngineOptions;

class Engine
{
public:
	Engine() = default;
	virtual ~Engine() = default;

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	Engine(Engine&&) = delete;
	Engine& operator=(Engine&&) = delete;

	std::expected<void, std::wstring> Initialize(const EngineOptions& options_);
	void Release();

	int Run();

	template <std::derived_from<SystemBase> TSystem>
	auto* AddSystem(this auto& self_);

	template <std::derived_from<SystemBase> TSystem>
	auto* GetSystem(this auto& self_);

private:
	std::unordered_map<std::type_index, std::unique_ptr<SystemBase>> systems;
};

template <std::derived_from<SystemBase> TSystem>
auto* Engine::AddSystem(this auto& self_)
{
	std::type_index index{ typeid(TSystem) };

	if (const auto it{ self_.systems.find(index) }; it != self_.systems.end())
	{
		return static_cast<TSystem*>(it->second.get());
	}

	auto [it, inserted] = self_.systems.emplace(index, std::make_unique<TSystem>());
	return static_cast<TSystem*>(it->second.get());
}

template <std::derived_from<SystemBase> TSystem>
auto* Engine::GetSystem(this auto& self_)
{
	const std::type_index index{ typeid(TSystem) };

	if (auto result{ self_.systems.find(index) }; result != self_.systems.end())
	{
		TSystem* system{ static_cast<TSystem*>(result->second.get()) };

		if constexpr (std::is_const_v<std::remove_reference_t<decltype(self_)>>)
		{
			return static_cast<const TSystem*>(system);
		}
		else
		{
			return system;
		}
	}

	if constexpr (std::is_const_v<std::remove_reference_t<decltype(self_)>>)
	{
		return static_cast<const TSystem*>(nullptr);
	}
	else
	{
		return static_cast<TSystem*>(nullptr);
	}
}
