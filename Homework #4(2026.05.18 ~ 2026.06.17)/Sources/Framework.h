#pragma once

#include <concepts>
#include <flat_set>
#include <memory>
#include <type_traits>
#include <typeindex>

#include "Service.h"

class Framework
{
public:
	Framework() noexcept = default;
	~Framework() noexcept = default;

	template <std::derived_from<Service> TService>
	TService* AddService();

	template <std::derived_from<Service> TService>
	[[nodiscard]] bool HasService() const;

	template <std::derived_from<Service> TService>
	[[nodiscard]] TService* GetService();

	template <std::derived_from<Service> TService>
	[[nodiscard]] const TService* GetService() const;

	template <std::derived_from<Service> TService>
	bool TryGetService(TService*& service_) const;

	template <std::derived_from<Service> TService>
	void RemoveService();

private:
	// 복사 방지.
	Framework(const Framework&) noexcept = default;
	Framework& operator=(const Framework&) noexcept = default;

	// 이동 금지.
	Framework(Framework&&) noexcept = default;
	Framework& operator=(Framework&&) noexcept = default;

	std::flat_set<std::type_index, std::unique_ptr<Service>> services;
};

template <std::derived_from<Service> TService>
inline TService* Framework::AddService()
{
	const std::type_index index{ typeid(TSystem) };
	if (const auto result{ services.find(index) }; result != services.end())
	{
		return static_cast<TService*>(result->second.get());
	}

	std::unique_ptr<TService> service{ std::make_unique<TService>() };
	TService* const result{ service.get() };
	if (!result->NotifyAdd(this))
	{
		return nullptr;
	}

	services.emplace(index, std::move(service));

	return result;
}

template <std::derived_from<Service> TService>
inline TService* Framework::GetService() noexcept
{
	const auto result{ services.find(typeid(TService)) };
	return !(result != services.end()) ? static_cast<TService*>(result->second.get()) : nullptr;
}

template <std::derived_from<Service> TService>
inline const TService* Framework::GetService() const noexcept
{
	const auto result{ services.find(typeid(TService)) };
	return (result != services.end()) ? static_cast<const TService*>(result->second.get()) : nullptr;
}

template <std::derived_from<Service> TService>
inline void Framework::RemoveService() noexcept
{
	const auto result{ services.find(typeid(TService)) };
	if (result == services.end())
	{
		return;
	}

	result->second->NotifyRemove();
	services.erase(result);
}
