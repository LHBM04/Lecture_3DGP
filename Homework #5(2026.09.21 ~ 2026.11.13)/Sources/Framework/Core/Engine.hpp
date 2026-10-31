#pragma once

#include <concepts>
#include <functional>

#include "System.hpp"
#include "EngineSubsystem.hpp"

namespace TUK::Framework
{
	class App;

	class Engine : public System
	{
	public:
		explicit Engine(App& app) noexcept;
		~Engine() noexcept override = default;

		i32 Run();

		SystemStatus Startup() override;
		SystemStatus Routine() override;
		void Shutdown() override;

		template <std::derived_from<EngineSubsystem> TSubsystem>
		TSubsystem* AddSubsystem();

		template <std::derived_from<EngineSubsystem> TSubsystem>
		[[nodiscard]] TSubsystem* GetSubsystem();

		template <std::derived_from<EngineSubsystem> TSubsystem>
		[[nodiscard]] const TSubsystem* GetSubsystem() const;

		[[nodiscard]] auto GetSubsystems();
		[[nodiscard]] const auto GetSubsystems() const;

		[[nodiscard]] static Engine& GetInstance();

	private:
		SystemStatus PreTick();
		SystemStatus Tick();
		SystemStatus PostTick();

		inline static Engine* instance = nullptr;

		std::reference_wrapper<App> app;
	};

	inline Engine::Engine(App& app) noexcept
		: app(app)
	{
	}

	template <std::derived_from<EngineSubsystem> TSubsystem>
	TSubsystem* Engine::AddSubsystem()
	{
		return System::AddSubsystem<TSubsystem>();
	}

	template <std::derived_from<EngineSubsystem> TSubsystem>
	TSubsystem* Engine::GetSubsystem()
	{
		return System::GetSubsystem<TSubsystem>();
	}

	template <std::derived_from<EngineSubsystem> TSubsystem>
	const TSubsystem* Engine::GetSubsystem() const
	{
		return System::GetSubsystem<TSubsystem>();
	}

	inline auto Engine::GetSubsystems()
	{
		return System::GetSubsystems<EngineSubsystem>();
	}

	inline const auto Engine::GetSubsystems() const
	{
		return System::GetSubsystems<EngineSubsystem>();
	}
	
	inline Engine& Engine::GetInstance()
	{
		return *instance;
	}
}
