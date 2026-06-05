#pragma once

#include <any>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>

#include "Service.h"
#include "Singleton.h"

class Framework : public Singleton<Framework>
{
public:
	Framework() noexcept = default;
	virtual ~Framework() noexcept = default;

	bool Initialize() noexcept;

	int Run() noexcept;
	void Shutdown() noexcept;

	template <class TOption>
	[[nodiscard]] const TOption& GetOption(std::wstring_view key_) const;

	template <class TOption>
	void SetOption(std::wstring_view key_, const TOption& value_);

	template <std::derived_from<Service> TService>
	TService& AddService();

	template <std::derived_from<Service> TService>
	[[nodiscard]] TService& GetService() const;

private:
	std::unordered_map<std::wstring, std::any> options;
	std::unordered_map<std::type_index, std::unique_ptr<Service>> services;
};

template <class TOption>
inline const TOption& Framework::GetOption(std::wstring_view key_) const
{
	const auto result{ options.find(std::wstring{ key_ }) };
	if (result == options.end())
	{
		throw std::runtime_error{ "Option not found." };
	}

	return std::any_cast<const TOption&>(result->second);
}

template <class TOption>
inline void Framework::SetOption(std::wstring_view key_, const TOption& value_)
{
	options[std::wstring{ key_ }] = value_;
}

template <std::derived_from<Service> TService>
inline TService& Framework::AddService()
{
	const std::type_index index{ typeid(TService) };
	if (services.contains(index))
	{
		return static_cast<TService&>(*services[index]);
	}

	auto [iter, inserted] { services.emplace(index, std::make_unique<TService>()) };
	iter->second->NotifyAdd(*this);
	return static_cast<TService&>(*(iter->second));
}

template <std::derived_from<Service> TService>
inline TService& Framework::GetService() const
{
	const auto result{ services.find(typeid(TService)) };
	if (result == services.end())
	{
		throw std::runtime_error{ "Service not found." };
	}

	return static_cast<TService&>(*(result->second));
}
