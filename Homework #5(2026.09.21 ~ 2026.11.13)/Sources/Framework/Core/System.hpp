#pragma once

#include <concepts>
#include <memory>
#include <ranges>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Subsystem.hpp"
#include "SystemStatus.hpp"

namespace TUK::Framework
{
	class System
	{
	public:
		System() noexcept = default;
		virtual ~System() noexcept = default;

		System(const System&) = delete;
		System& operator=(const System&) = delete;

		System(System&&) = delete;
		System& operator=(System&&) = delete;

		virtual SystemStatus Startup();
		virtual SystemStatus Routine();
		virtual void Shutdown();

	protected:
		template <std::derived_from<Subsystem> TSubsystem>
		TSubsystem* AddSubsystem();

		template <std::derived_from<Subsystem> TSubsystem>
		[[nodiscard]] TSubsystem* GetSubsystem() noexcept;

		template <std::derived_from<Subsystem> TSubsystem>
		[[nodiscard]] const TSubsystem* GetSubsystem() const noexcept;

		template <std::derived_from<Subsystem> TSubsystem>
		[[nodiscard]] auto GetSubsystems() noexcept;

		template <std::derived_from<Subsystem> TSubsystem>
		[[nodiscard]] auto GetSubsystems() const noexcept;

	private:
		std::vector<std::unique_ptr<Subsystem>> subsystems;
		std::unordered_map<std::type_index, Subsystem*> subsystemsByType;
	};

	template <std::derived_from<Subsystem> TSubsystem>
	TSubsystem* System::AddSubsystem()
	{
		const auto result = subsystemsByType.find(std::type_index(typeid(TSubsystem)));
		if (result != subsystemsByType.end())
		{
			return dynamic_cast<TSubsystem*>(result->second);
		}

		auto subsystem = std::make_unique<TSubsystem>();
		auto* pointer = subsystem.get();
		subsystems.push_back(std::move(subsystem));
		subsystemsByType.emplace(std::type_index(typeid(TSubsystem)), pointer);
		return pointer;
	}

	template <std::derived_from<Subsystem> TSubsystem>
	TSubsystem* System::GetSubsystem() noexcept
	{
		const auto result = subsystemsByType.find(std::type_index(typeid(TSubsystem)));
		if (result != subsystemsByType.end())
		{
			return static_cast<TSubsystem*>(result->second);
		}

		return nullptr;
	}

	template <std::derived_from<Subsystem> TSubsystem>
	const TSubsystem* System::GetSubsystem() const noexcept
	{
		const auto result = subsystemsByType.find(std::type_index(typeid(TSubsystem)));
		if (result != subsystemsByType.end())
		{
			return static_cast<const TSubsystem*>(result->second);
		}

		return nullptr;
	}

	template <std::derived_from<Subsystem> TSubsystem>
	auto System::GetSubsystems() noexcept
	{
		return subsystems | std::views::filter([](const auto& subsystem)
			{
				return dynamic_cast<TSubsystem*>(subsystem.get()) != nullptr;
			})
			| std::views::transform([](const auto& subsystem) -> TSubsystem*
			{
				return static_cast<TSubsystem*>(subsystem.get());
			});
	}

	template <std::derived_from<Subsystem> TSubsystem>
	auto System::GetSubsystems() const noexcept
	{
		return subsystems | std::views::filter([](const auto& subsystem)
			{
				return dynamic_cast<const TSubsystem*>(subsystem.get()) != nullptr;
			})
			| std::views::transform([](const auto& subsystem) -> const TSubsystem*
			{
				return static_cast<const TSubsystem*>(subsystem.get());
			});
	}
}
