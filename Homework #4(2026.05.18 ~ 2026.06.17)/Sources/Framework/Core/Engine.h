#pragma once

#include "EngineSubsystem.h"
#include "System.h"

namespace TUK::Framework
{
	class Game;

	class Engine : public System
	{
	public:
		Engine();
		~Engine() override;

		[[nodiscard]] static Engine& GetInstance();
		
		int Run(Game& game);

		template <std::derived_from<EngineSubsystem> TSubsystem>
		TSubsystem* AddSubsystem();

		template <std::derived_from<EngineSubsystem> TSubsystem>
		[[nodiscard]] TSubsystem* GetSubsystem();

		template <std::derived_from<EngineSubsystem> TSubsystem>
		[[nodiscard]] const TSubsystem* GetSubsystem() const;

		template <std::derived_from<EngineSubsystem> TSubsystem>
		[[nodiscard]] auto GetSubsystems();

		template <std::derived_from<EngineSubsystem> TSubsystem>
		[[nodiscard]] const auto GetSubsystems() const;

	private:
		void OnPreTick();
		void OnTick(Game& game);
		void OnPostTick();

		static Engine* instance;
	};

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

	template <std::derived_from<EngineSubsystem> TSubsystem>
	auto Engine::GetSubsystems()
	{
		return System::GetSubsystems<TSubsystem>();
	}

	template <std::derived_from<EngineSubsystem> TSubsystem>
	const auto Engine::GetSubsystems() const
	{
		return System::GetSubsystems<TSubsystem>();
	}
}