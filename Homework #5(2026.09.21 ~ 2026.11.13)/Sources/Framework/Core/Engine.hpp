#pragma once

#include <concepts>

#include "System.hpp"
#include "EngineSubsystem.hpp"

namespace TUK::Framework
{
	class App;

	class Engine : public System
	{
	public:
		explicit Engine(App* app) noexcept;

		[[nodiscard]] i32 Run();

		SystemStatus Startup() override;
		SystemStatus Routine() override;
		void Shutdown() override;

		template <std::derived_from<EngineSubsystem> TSubsystem>
		TSubsystem* AddSubsystem();

		template <std::derived_from<EngineSubsystem> TSubsystem>
		[[nodiscard]] TSubsystem* GetSubsystem() noexcept;

		template <std::derived_from<EngineSubsystem> TSubsystem>
		[[nodiscard]] const TSubsystem* GetSubsystem() const noexcept;

		[[nodiscard]] auto GetSubsystems() noexcept;
		[[nodiscard]] auto GetSubsystems() const noexcept;

		[[nodiscard]] static Engine& GetInstance() noexcept;

	private:
		SystemStatus PreTick();
		SystemStatus Tick();
		SystemStatus PostTick();

		inline static Engine* instance = nullptr;

		App* app = nullptr;
	};

	inline Engine::Engine(App* app) noexcept
		: app(app)
	{
	}

	template <std::derived_from<EngineSubsystem> TSubsystem>
	TSubsystem* Engine::AddSubsystem()
	{
		return System::AddSubsystem<TSubsystem>();
	}

	template <std::derived_from<EngineSubsystem> TSubsystem>
	TSubsystem* Engine::GetSubsystem() noexcept
	{
		return System::GetSubsystem<TSubsystem>();
	}

	template <std::derived_from<EngineSubsystem> TSubsystem>
	const TSubsystem* Engine::GetSubsystem() const noexcept
	{
		return System::GetSubsystem<TSubsystem>();
	}

	inline auto Engine::GetSubsystems() noexcept
	{
		return System::GetSubsystems<EngineSubsystem>();
	}

	inline auto Engine::GetSubsystems() const noexcept
	{
		return System::GetSubsystems<EngineSubsystem>();
	}
	
	inline Engine& Engine::GetInstance() noexcept
	{
		return *instance;
	}
}
