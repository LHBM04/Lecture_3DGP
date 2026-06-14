#pragma once

#include <any>
#include <concepts>
#include <flat_map>
#include <memory>
#include <string>
#include <string_view>
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

    bool Initialize();
    
    int Run();
    void Quit(int exitCode_);

    template <class TOption>
    void SetOption(std::string_view key_, TOption value_);

    template <class TOption>
    TOption GetOption(std::string_view key_, TOption default_);

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
    std::flat_map<std::string, std::any> options;
    std::flat_map<std::type_index, std::unique_ptr<Service>> services;
};

template <class TOption>
inline TOption Framework::GetOption(std::string_view key_, TOption default_)
{
    std::string key{ key_ };
    if (!options.contains(key))
    {
        return default_;
    }

    return std::any_cast<TOption>(options[key]);
}

template <class TOption>
inline void Framework::SetOption(std::string_view key_, TOption value_)
{
    options[std::string(key_)] = value_;
}

template <std::derived_from<Service> TService>
inline TService* Framework::AddService()
{
    const std::type_index index{ typeid(TService) };

    if (const auto result{ services.find(index) }; result != services.end())
    {
        return static_cast<TService*>(result->second.get());
    }

    const auto [result, isInserted] { services.emplace(index, std::make_unique<TService>())};

    result->second->NotifyAdd(this);
    return static_cast<TService*>(result->second.get());
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
