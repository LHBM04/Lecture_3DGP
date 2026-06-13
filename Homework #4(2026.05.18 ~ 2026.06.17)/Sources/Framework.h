#pragma once

#include <concepts>
#include <flat_map>
#include <memory>
#include <typeindex>

#include "Service.h"

class Framework
{
public:
    Framework() = default;
    ~Framework() = default;

    Framework(const Framework&) = delete;
    Framework& operator=(const Framework&) = delete;

    Framework(Framework&&) = delete;
    Framework& operator=(Framework&&) = delete;

    template <std::derived_from<Service> TService>
    TService* AddService();

    template <std::derived_from<Service> TService>
    [[nodiscard]] bool HasService() const;

    template <std::derived_from<Service> TService>
    [[nodiscard]] TService* GetService();

    template <std::derived_from<Service> TService>
    [[nodiscard]] const TService* GetService() const;

    template <std::derived_from<Service> TService>
    void RemoveService();

private:
    std::flat_map<std::type_index, std::unique_ptr<Service>> services;
};

template <std::derived_from<Service> TService>
inline TService* Framework::AddService()
{
    const std::type_index index{ typeid(TService) };

    if (const auto result{ services.find(index) }; result != services.end())
    {
        return static_cast<TService*>(result->second.get());
    }

    auto service{ std::make_unique<TService>() };
    TService* const result{ service.get() };

    const auto [result, isInserted] = services.emplace(index, std::move(service));

    if (!isInserted)
    {
        return static_cast<TService*>(result->second.get());
    }

    if (!result->NotifyAdd(this))
    {
        services.erase(result);
        return nullptr;
    }

    return result;
}

template <std::derived_from<Service> TService>
inline bool Framework::HasService() const
{
    return services.contains(std::type_index{ typeid(TService) });
}

template <std::derived_from<Service> TService>
inline TService* Framework::GetService()
{
    const auto result{ services.find(std::type_index{ typeid(TService) }) };

    if (result == services.end())
    {
        return nullptr;
    }

    return static_cast<TService*>(result->second.get());
}

template <std::derived_from<Service> TService>
inline const TService* Framework::GetService() const
{
    const auto result{ services.find(std::type_index{ typeid(TService) }) };

    if (result == services.end())
    {
        return nullptr;
    }

    return static_cast<const TService*>(result->second.get());
}

template <std::derived_from<Service> TService>
inline void Framework::RemoveService()
{
    const auto result{ services.find(std::type_index{ typeid(TService) }) };

    if (result == services.end())
    {
        return;
    }

    result->second->NotifyRemove();
    services.erase(result);
}
