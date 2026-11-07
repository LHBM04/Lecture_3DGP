#pragma once

#include <concepts>
#include <memory>
#include <memory_resource>
#include <ranges>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Subsystem.hpp"
#include "SystemStatus.hpp"

namespace TUK::Framework
{
	template <std::derived_from<Subsystem> TSubsystem>
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
		template <std::derived_from<TSubsystem> TResult>
		TResult* AddSubsystem();

		template <std::derived_from<TSubsystem> TResult>
		[[nodiscard]] TResult* GetSubsystem() noexcept;

		template <std::derived_from<TSubsystem> TResult>
		[[nodiscard]] const TResult* GetSubsystem() const noexcept;

		[[nodiscard]] auto GetSubsystems() noexcept;
		[[nodiscard]] auto GetSubsystems() const noexcept;

	private:
		std::pmr::vector<std::unique_ptr<Subsystem>> subsystems;
		std::pmr::unordered_map<std::type_index, Subsystem*> subsystemsByType;
	};

	template <std::derived_from<Subsystem> TSubsystem>
	SystemStatus System<TSubsystem>::Startup()
	{
		std::ranges::sort(subsystems, std::ranges::less{}, &Subsystem::GetPriority);

		for (const std::unique_ptr<Subsystem>& subsystem : subsystems)
		{
			if (subsystem->OnStartup() == SystemStatus::FAILURE)
			{
				return SystemStatus::FAILURE;
			}
		}

		return SystemStatus::CONTINUE;
	}

	template <std::derived_from<Subsystem> TSubsystem>
	SystemStatus System<TSubsystem>::Routine()
	{
		// 일관성을 위해서 그냥 구현함.
		return SystemStatus::CONTINUE;
	}

	template <std::derived_from<Subsystem> TSubsystem>
	void System<TSubsystem>::Shutdown()
	{
		for (const std::unique_ptr<Subsystem>& subsystem : subsystems | std::views::reverse)
		{
			subsystem->OnShutdown();
		}

		subsystems.clear();
		subsystemsByType.clear();
	}

	template <std::derived_from<Subsystem> TSubsystem>
	template <std::derived_from<TSubsystem> TResult>
	TResult* System<TSubsystem>::AddSubsystem()
	{
		const auto result = subsystemsByType.find(std::type_index(typeid(TResult)));
		if (result != subsystemsByType.end())
		{
			return dynamic_cast<TResult*>(result->second);
		}

		auto subsystem = std::make_unique<TResult>();
		auto* pointer = subsystem.get();
		subsystems.push_back(std::move(subsystem));
		subsystemsByType.emplace(std::type_index(typeid(TResult)), pointer);
		return pointer;
	}

	template <std::derived_from<Subsystem> TSubsystem>
	template <std::derived_from<TSubsystem> TResult>
	TResult* System<TSubsystem>::GetSubsystem() noexcept
	{
		const auto result = subsystemsByType.find(std::type_index(typeid(TResult)));
		if (result != subsystemsByType.end())
		{
			return static_cast<TResult*>(result->second);
		}

		return nullptr;
	}

	template <std::derived_from<Subsystem> TSubsystem>
	template <std::derived_from<TSubsystem> TResult>
	const TResult* System<TSubsystem>::GetSubsystem() const noexcept
	{
		const auto result = subsystemsByType.find(std::type_index(typeid(TResult)));
		if (result != subsystemsByType.end())
		{
			return static_cast<const TResult*>(result->second);
		}

		return nullptr;
	}

	template <std::derived_from<Subsystem> TSubsystem>
	auto System<TSubsystem>::GetSubsystems() noexcept
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
	auto System<TSubsystem>::GetSubsystems() const noexcept
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
