#pragma once

#include <any>
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
#include <vector>

#include "Subsystem.h"

namespace TUK::Framework
{
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

		template <class TOption, class TValue>
		void AddOption(std::string_view key, TValue&& value);

		template <class TOption>
		[[nodiscard]] TOption& GetOption(const std::string& key);

		template <class TOption>
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
		std::unordered_map<std::string, std::any> options;
		
		/** 포함된 서브시스템 */
		std::vector<std::unique_ptr<Subsystem>> subsystems;
		std::unordered_map<std::type_index, std::reference_wrapper<Subsystem>> subsystemsByType;
	};

	template <class TOption, class TValue>
	void System::AddOption(std::string_view key, TValue&& value)
	{
		assert(!options.contains(key.data()) && "The option must not already be registered.");
		options.try_emplace(key.data(), std::in_place_type<TOption>, std::forward<TValue>(value));
	}

	template <class TOption>
	TOption& System::GetOption(const std::string& key)
	{
		const auto result = options.find(key);
		

		auto* value = std::any_cast<TOption>(&result->second);
		assert(value && "The option type must match its registered type.");

		return *value;
	}

	template <class TOption>
	const TOption& System::GetOption(const std::string& key) const
	{
		const auto result = options.find(key);
		assert(result != options.end() && "The requested option must be registered.");

		const auto* value = std::any_cast<TOption>(&result->second);
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
		return subsystems | std::views::filter([](const auto& subsystem) { return dynamic_cast<TSubsystem*>(subsystem.get()) != nullptr; })
						  | std::views::transform([](const auto& subsystem) -> TSubsystem& { return static_cast<TSubsystem&>(*subsystem); });
	}

	template <std::derived_from<Subsystem> TSubsystem>
	const auto System::GetSubsystems() const
	{
		return subsystems | std::views::filter([](const auto& subsystem) { return dynamic_cast<TSubsystem*>(subsystem.get()) != nullptr; })
						  | std::views::transform([](const auto& subsystem) -> const TSubsystem& { return static_cast<const TSubsystem&>(*subsystem); });
	}
}

