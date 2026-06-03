#pragma once

#include <any>
#include <concepts>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "Requests.h"
#include "Service.h"

class RenderService;
class ResourceService;
class SceneService;
class WindowService;

class Engine
{
public:
	Engine() = default;
	~Engine() = default;

    bool Initialize();
    void Shutdown();

    int Run();

    template <std::derived_from<Service> TService>
    TService& AddService();

    template <std::derived_from<Service> TService>
    TService& GetService() const;

    template <class TOption>
    TOption GetOption(std::wstring_view key_, TOption default_ = {}) const;

    template <class TOption>
    void SetOption(std::wstring_view key_, TOption value_);

private:
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	Engine(Engine&&) = delete;
	Engine& operator=(Engine&&) = delete;

    std::unordered_map<std::wstring, std::any> options;
    std::vector<std::unique_ptr<Service>> services;
};

template <std::derived_from<Service> TService>
TService& Engine::AddService()
{
    auto service{ std::make_unique<TService>() };
    TService& ref{ *service };

    services.push_back(std::move(service));

    ref.NotifyAdd(this);
    return ref;
}

template <std::derived_from<Service> TService>
TService& Engine::GetService() const
{
    for (const auto& service : services)
    {
        if (TService* casted{ dynamic_cast<TService*>(service.get()) })
        {
            return *casted;
        }
    }

    throw std::runtime_error("요청한 서비스를 찾을 수 없습니다.");
}

template <class TOption>
TOption Engine::GetOption(std::wstring_view key_, TOption default_) const
{
    auto result{ options.find(std::wstring(key_)) };
    if (result != options.end())
    {
        try
        {
            return std::any_cast<TOption>(result->second);
        }
        catch (const std::bad_any_cast&)
        {
            return default_;
        }
    }

    return default_;
}

template <class TOption>
void Engine::SetOption(std::wstring_view key_, TOption value_)
{
    options[std::wstring(key_)] = value_;
}
