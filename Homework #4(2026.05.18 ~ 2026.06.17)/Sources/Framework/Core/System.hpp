#pragma once

#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
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

#include "Subsystem.hpp"
#include "SystemStatus.hpp"

namespace TUK::Framework
{
	class System
	{
	public:
		virtual ~System() noexcept;

		System(const System&) = delete;
		System& operator=(const System&) = delete;
		
		System(System&&) = delete;
		System& operator=(System&&) = delete;

		virtual SystemStatus Startup();
		virtual SystemStatus Routine();
		virtual void Shutdown();

	protected:
		System() noexcept;

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
		/** 포함된 서브시스템 */
		std::pmr::vector<std::unique_ptr<Subsystem>> subsystems;
		std::pmr::unordered_map<std::type_index, std::reference_wrapper<Subsystem>> subsystemsByType;
	};

	template <std::derived_from<Subsystem> TSubsystem>
	TSubsystem* System::AddSubsystem()
	{
		const auto result = subsystemsByType.find(typeid(TSubsystem));
		if (result != subsystemsByType.end())
		{
			return dynamic_cast<TSubsystem*>(&result->second.get());
		}

		auto subsystem = std::unique_ptr<TSubsystem>(new TSubsystem());
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
		if (result != subsystemsByType.end())
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

