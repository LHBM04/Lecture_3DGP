#pragma once

#include <cassert>
#include <concepts>
#include <cstddef>
#include <functional>
#include <memory>
#include <ranges>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "Subsystem.h"

namespace TUK::Framework
{

	/** 조회 시에는 실제 저장 타입을 사용한다. */
	template <class T>
	concept OptionType = std::same_as<T, std::string>
					  || std::same_as<T, std::string_view>
					  || std::same_as<T, std::wstring_view>
					  || std::same_as<T, int>
					  || std::same_as<T, float>
					  || std::same_as<T, bool>;

	class System
	{
	public:
		System();
		virtual ~System();

		System(const System&) = delete;
		System& operator=(const System&) = delete;
		
		System(System&&) = delete;
		System& operator=(System&&) = delete;

		void RequestQuit(int code) noexcept;
		void ReportError(std::string_view message);
		[[nodiscard]] bool IsRunning() const noexcept;
		[[nodiscard]] int GetQuitCode() const noexcept;

		template <class TValue>
		void AddOption(std::string_view key, TValue value);

		/** 옵션을 추가하거나 기존 값을 덮어쓴다. */
		template <class TValue>
		void SetOption(std::string_view key, TValue value) = delete;

		template <OptionType TOption>
		[[nodiscard]] TOption& GetOption(const std::string& key);

		template <OptionType TOption>
		[[nodiscard]] const TOption& GetOption(const std::string& key) const;

	protected:
		void Startup();
		void Shutdown();

		template <std::derived_from<Subsystem> TSubsystem>
		TSubsystem* AddSubsystem();

		template <std::derived_from<Subsystem> TSubsystem>
		[[nodiscard]] TSubsystem* GetSubsystem();

		template <std::derived_from<Subsystem> TSubsystem>
		[[nodiscard]] const TSubsystem* GetSubsystem() const;

		template <std::derived_from<Subsystem> TSubsystem>
		[[nodiscard]] auto GetSubsystems();

		template <std::derived_from<Subsystem> TSubsystem>
		[[nodiscard]] const auto GetSubsystems() const;

	private:
		/** 시스템 상태 */
		bool isRunning;
		int quitCode;
		
		/** 시스템 옵션 */
		std::unordered_map<std::string, std::variant<std::string, int, float, bool>> options;
		
		/** 포함된 서브시스템 */
		std::vector<std::unique_ptr<Subsystem>> subsystems;
		std::unordered_map<std::type_index, std::reference_wrapper<Subsystem>> subsystemsByType;
	};

	template <>
	inline void System::SetOption<std::string>(std::string_view key, std::string value)
	{
		const auto separator = key.find('.');
		assert(separator != std::string_view::npos && separator > 0 && separator + 1 < key.size());
		options.insert_or_assign(std::string(key),
			std::variant<std::string, int, float, bool>(std::in_place_type<std::string>, std::move(value)));
	}

	template <>
	inline void System::SetOption<int>(std::string_view key, int value)
	{
		const auto separator = key.find('.');
		assert(separator != std::string_view::npos && separator > 0 && separator + 1 < key.size());
		options.insert_or_assign(std::string(key),
			std::variant<std::string, int, float, bool>(std::in_place_type<int>, std::move(value)));
	}

	template <>
	inline void System::SetOption<float>(std::string_view key, float value)
	{
		const auto separator = key.find('.');
		assert(separator != std::string_view::npos && separator > 0 && separator + 1 < key.size());
		options.insert_or_assign(std::string(key),
			std::variant<std::string, int, float, bool>(std::in_place_type<float>, std::move(value)));
	}

	template <>
	inline void System::SetOption<bool>(std::string_view key, bool value)
	{
		const auto separator = key.find('.');
		assert(separator != std::string_view::npos && separator > 0 && separator + 1 < key.size());
		options.insert_or_assign(std::string(key),
			std::variant<std::string, int, float, bool>(std::in_place_type<bool>, std::move(value)));
	}

	template <class TValue>
	void System::AddOption(std::string_view key, TValue value)
	{
		assert(!options.contains(std::string(key)) && "The option must not already be registered.");
		SetOption<TValue>(key, std::move(value));
	}

	template <OptionType TOption>
	TOption& System::GetOption(const std::string& key)
	{
		const auto result = options.find(key);
		assert(result != options.end() && "The requested option must be registered.");

		auto* value = std::get_if<TOption>(&result->second);
		assert(value && "The option type must match its registered type.");
		return *value;
	}

	template <OptionType TOption>
	const TOption& System::GetOption(const std::string& key) const
	{
		const auto result = options.find(key);
		assert(result != options.end() && "The requested option must be registered.");

		const auto* value = std::get_if<TOption>(&result->second);
		assert(value && "The option type must match its registered type.");
		return *value;
	}

	template <std::derived_from<Subsystem> TSubsystem>
	TSubsystem* System::AddSubsystem()
	{
		const auto result = subsystemsByType.find(typeid(TSubsystem));
		if (result != subsystemsByType.end())
		{
			return dynamic_cast<TSubsystem*>(&result->second.get());
		}

		auto subsystem = std::make_unique<TSubsystem>();
		auto* pointer = subsystem.get();
		subsystems.push_back(std::move(subsystem));
		subsystemsByType.emplace(typeid(TSubsystem), std::ref(*pointer));
		return pointer;
	}

	template <std::derived_from<Subsystem> TSubsystem>
	TSubsystem* System::GetSubsystem()
	{
		const auto result = subsystemsByType.find(typeid(TSubsystem));
		if (result != subsystemsByType.end())
		{
			return static_cast<TSubsystem*>(&result->second.get());
		}

		return nullptr;
	}

	template <std::derived_from<Subsystem> TSubsystem>
	const TSubsystem* System::GetSubsystem() const
	{
		const auto result = subsystemsByType.find(typeid(TSubsystem));
		if (result == subsystemsByType.end())
		{
			return static_cast<const TSubsystem*>(&result->second.get());
		}

		return nullptr;
	}

	template <std::derived_from<Subsystem> TSubsystem>
	auto System::GetSubsystems()
	{
		return subsystems | std::views::filter([](const auto& subsystem) {
								return dynamic_cast<TSubsystem*>(subsystem.get()) != nullptr;
							})
						  | std::views::transform([](const auto& subsystem) -> TSubsystem& {
							  return static_cast<TSubsystem&>(*subsystem);
							});
	}

	template <std::derived_from<Subsystem> TSubsystem>
	const auto System::GetSubsystems() const
	{
		return subsystems | std::views::filter([](const auto& subsystem) {
								return dynamic_cast<TSubsystem*>(subsystem.get()) != nullptr;
							})
						  | std::views::transform([](const auto& subsystem) -> const TSubsystem& {
							  return static_cast<const TSubsystem&>(*subsystem);
							});
	}
}

