#pragma once

#include <any>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Singleton.h"
#include "Service.h"

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
	void SetOption(std::wstring_view key, const TOption& value_);

	template <std::derived_from<Service> TService>
	[[nodiscard]] TService& GetService() const;

private:
	std::unordered_map<std::wstring, std::any> options;
};

template <class TOption>
inline const TOption& Framework::GetOption(std::wstring_view key_) const noexcept
{
	if (const auto iter{ options.find(key_) }; iter == options.end())
	{
		throw std::runtime_error{ "Option not found." };
	}

	return std::any_cast<const TOption&>(iter->second);
}

template <class TOption>
inline void Framework::SetOption(std::wstring_view key, const TOption& value_) noexcept
{
	options[key] = value_;
}
