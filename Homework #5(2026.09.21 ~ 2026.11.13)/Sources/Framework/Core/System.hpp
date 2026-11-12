#pragma once

#include <concepts>
#include <functional>
#include <memory>
#include <ranges>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Common.hpp"
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
		void RemoveSubsystem();

		template <std::derived_from<TSubsystem> TResult>
		[[nodiscard]] TResult* GetSubsystem() noexcept;

		template <std::derived_from<TSubsystem> TResult>
		[[nodiscard]] const TResult* GetSubsystem() const noexcept;

		[[nodiscard]] auto GetSubsystems() noexcept;
		[[nodiscard]] auto GetSubsystems() const noexcept;

		template <std::derived_from<TSubsystem> TResult>
		Delegate<TSubsystem*>::ListenerId OnSubsystemAdded(Action<TResult*> callback);

		template <std::derived_from<TSubsystem> TResult, class TObject>
		Delegate<TSubsystem*>::ListenerId OnSubsystemAdded(TObject* object, void (TObject::*method)(TResult*));

		template <std::derived_from<TSubsystem> TResult>
		Delegate<TSubsystem*>::ListenerId OnSubsystemRemoved(Action<TResult*> callback);

		template <std::derived_from<TSubsystem> TResult, class TObject>
		Delegate<TSubsystem*>::ListenerId OnSubsystemRemoved(TObject* object, void (TObject::*method)(TResult*));

	private:
		std::vector<std::unique_ptr<TSubsystem>> subsystems;
		std::unordered_map<std::type_index, TSubsystem*> subsystemsByType;
		std::unordered_map<std::type_index, Delegate<TSubsystem*>> onSubsystemAdded;
		std::unordered_map<std::type_index, Delegate<TSubsystem*>> onSubsystemRemoved;
	};

	template <std::derived_from<Subsystem> TSubsystem>
	SystemStatus System<TSubsystem>::Startup()
	{
		std::ranges::sort(subsystems, std::ranges::less{}, &Subsystem::GetPriority);

		for (const std::unique_ptr<TSubsystem>& subsystem : subsystems)
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
		for (const std::unique_ptr<TSubsystem>& subsystem : subsystems | std::views::reverse)
		{
			TSubsystem* pointer = subsystem.get();
			const std::type_index type(typeid(*pointer));
			if (const auto result = onSubsystemRemoved.find(type); result != onSubsystemRemoved.end())
			{
				result->second.Broadcast(pointer);
			}
			pointer->OnShutdown();
		}

		for (auto& [type, delegate] : onSubsystemAdded)
		{
			delegate.Clear();
		}
		for (auto& [type, delegate] : onSubsystemRemoved)
		{
			delegate.Clear();
		}
		onSubsystemAdded.clear();
		onSubsystemRemoved.clear();
		subsystems.clear();
		subsystemsByType.clear();
	}

	template <std::derived_from<Subsystem> TSubsystem>
	template <std::derived_from<TSubsystem> TResult>
	TResult* System<TSubsystem>::AddSubsystem()
	{
		const std::type_index type(typeid(TResult));
		const auto result = subsystemsByType.find(type);
		if (result != subsystemsByType.end())
		{
			return static_cast<TResult*>(result->second);
		}

		auto subsystem = std::make_unique<TResult>();
		auto* pointer = subsystem.get();
		subsystems.push_back(std::move(subsystem));
		subsystemsByType.emplace(type, pointer);

		if (const auto found = onSubsystemAdded.find(type); found != onSubsystemAdded.end())
		{
			found->second.Broadcast(pointer);
		}
		return pointer;
	}

	template <std::derived_from<Subsystem> TSubsystem>
	template <std::derived_from<TSubsystem> TResult>
	void System<TSubsystem>::RemoveSubsystem()
	{
		const std::type_index type(typeid(TResult));
		const auto result = subsystemsByType.find(type);
		if (result == subsystemsByType.end())
		{
			return;
		}

		TSubsystem* pointer = result->second;
		if (const auto found = onSubsystemRemoved.find(type); found != onSubsystemRemoved.end())
		{
			found->second.Broadcast(pointer);
		}
		pointer->OnShutdown();

		subsystemsByType.erase(result);
		std::erase_if(subsystems, [pointer](const std::unique_ptr<TSubsystem>& subsystem)
		{
			return subsystem.get() == pointer;
		});
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
		return subsystems | std::views::transform([](const auto& subsystem) -> TSubsystem*
		{
			return subsystem.get();
		});
	}

	template <std::derived_from<Subsystem> TSubsystem>
	auto System<TSubsystem>::GetSubsystems() const noexcept
	{
		return subsystems | std::views::transform([](const auto& subsystem) -> const TSubsystem*
		{
			return subsystem.get();
		});
	}

	template <std::derived_from<Subsystem> TSubsystem>
	template <std::derived_from<TSubsystem> TResult>
	Delegate<TSubsystem*>::ListenerId System<TSubsystem>::OnSubsystemAdded(Action<TResult*> callback)
	{
		return onSubsystemAdded[std::type_index(typeid(TResult))].AddListener(
			[callback = std::move(callback)](TSubsystem* subsystem)
			{
				if (callback)
				{
					callback(static_cast<TResult*>(subsystem));
				}
			});
	}

	template <std::derived_from<Subsystem> TSubsystem>
	template <std::derived_from<TSubsystem> TResult, class TObject>
	Delegate<TSubsystem*>::ListenerId System<TSubsystem>::OnSubsystemAdded(TObject* object, void (TObject::*method)(TResult*))
	{
		return OnSubsystemAdded<TResult>(std::bind_front(method, object));
	}

	template <std::derived_from<Subsystem> TSubsystem>
	template <std::derived_from<TSubsystem> TResult>
	Delegate<TSubsystem*>::ListenerId System<TSubsystem>::OnSubsystemRemoved(Action<TResult*> callback)
	{
		return onSubsystemRemoved[std::type_index(typeid(TResult))].AddListener(
			[callback = std::move(callback)](TSubsystem* subsystem)
			{
				if (callback)
				{
					callback(static_cast<TResult*>(subsystem));
				}
			});
	}

	template <std::derived_from<Subsystem> TSubsystem>
	template <std::derived_from<TSubsystem> TResult, class TObject>
	Delegate<TSubsystem*>::ListenerId System<TSubsystem>::OnSubsystemRemoved(TObject* object, void (TObject::*method)(TResult*))
	{
		return OnSubsystemRemoved<TResult>(std::bind_front(method, object));
	}
}
