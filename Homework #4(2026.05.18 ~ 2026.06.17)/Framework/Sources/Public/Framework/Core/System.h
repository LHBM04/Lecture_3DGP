#pragma once

#include <concepts>
#include <expected>
#include <functional>
#include <memory>
#include <system_error>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Subsystem.h"

namespace TUK::Framework
{
	template <class TSubsystem>
	concept FromSubsystem = std::derived_from<TSubsystem, Subsystem>;

	template <class TSubsystem>
	concept PreTickableSubsystem = requires(TSubsystem updatable, float deltaTime)
	{
		{ updatable.OnPreTick() } -> std::same_as<void>;
	};

	template <class TSubsystem>
	concept TickableSubsystem = requires(TSubsystem updatable, float deltaTime)
	{
		{ updatable.OnTick() } -> std::same_as<void>;
	};

	template <class TSubsystem>
	concept PostTickableSubsystem = requires(TSubsystem updatable, float deltaTime)
	{
		{ updatable.OnPostTick() } -> std::same_as<void>;
	};

	class System
	{
	public:
		System() noexcept;
		virtual ~System() noexcept = default;

		System(const System&) = delete;
		System& operator=(const System&) = delete;

		System(System&&) = delete;
		System& operator=(System&&) = delete;

		void Startup();
		void Shutdown();

		void Run();

		template <FromSubsystem TSubsystem>
		std::expected<std::reference_wrapper<TSubsystem>, std::error_code> AddSubsystem();

		template <FromSubsystem TSubsystem>
		[[nodiscard]] std::expected<std::reference_wrapper<TSubsystem>, std::error_code> GetSubsystem();

		template <FromSubsystem TSubsystem>
		[[nodiscard]] std::expected<std::reference_wrapper<const TSubsystem>, std::error_code> GetSubsystem() const;

		[[nodiscard]] static System& GetInstance() noexcept;

		[[nodiscard]] bool IsRunning() const noexcept;
		void RequestQuit() noexcept;

	private:
		static std::unique_ptr<System> instance;

		bool isRunning;

		std::vector<std::unique_ptr<Subsystem>> subsystems;
		std::unordered_map<std::type_index, Subsystem*> subsystemMap;

		std::vector<std::function<void()>> preTickableSubsystems;
		std::vector<std::function<void()>> tickableSubsystems;
		std::vector<std::function<void()>> postTickableSubsystems;
	};

	template <FromSubsystem TSubsystem>
	std::expected<std::reference_wrapper<TSubsystem>, std::error_code> System::AddSubsystem()
	{
		if (subsystemMap.contains(typeid(TSubsystem)))
		{
			return std::unexpected(std::make_error_code(std::errc::operation_not_permitted));
		}

		std::unique_ptr<TSubsystem> subsystem = std::make_unique<TSubsystem>();

		if constexpr (PreTickableSubsystem<TSubsystem>)
		{
			preTickableSubsystems.emplace_back(
				[ptr = subsystem.get()]
				{
					ptr->OnPreTick();
				}
			);
		}
		if constexpr (TickableSubsystem<TSubsystem>)
		{
			tickableSubsystems.emplace_back(
				[ptr = subsystem.get()]
				{
					ptr->OnTick();
				}
			);
		}
		if constexpr (PostTickableSubsystem<TSubsystem>)
		{
			postTickableSubsystems.emplace_back(
				[ptr = subsystem.get()]
				{
					ptr->OnPostTick();
				}
			);
		}

		subsystems.emplace_back(std::move(subsystem));
		subsystemMap[typeid(TSubsystem)] = subsystems.back().get();

		return std::ref(static_cast<TSubsystem&>(*subsystemMap[typeid(TSubsystem)]));
	}

	template <FromSubsystem TSubsystem>
	std::expected<std::reference_wrapper<TSubsystem>, std::error_code> System::GetSubsystem()
	{
		auto result = subsystemMap.find(typeid(TSubsystem));
		if (result != subsystemMap.end())
		{
			return std::ref(static_cast<TSubsystem&>(*result->second));
		}

		return std::unexpected(std::make_error_code(std::errc::operation_not_permitted));
	}

	template <FromSubsystem TSubsystem>
	std::expected<std::reference_wrapper<const TSubsystem>, std::error_code> System::GetSubsystem() const
	{
		auto result = subsystemMap.find(typeid(TSubsystem));
		if (result != subsystemMap.end())
		{
			return std::ref(static_cast<TSubsystem&>(*result->second));
		}

		return std::unexpected(std::make_error_code(std::errc::operation_not_permitted));
	}
}
