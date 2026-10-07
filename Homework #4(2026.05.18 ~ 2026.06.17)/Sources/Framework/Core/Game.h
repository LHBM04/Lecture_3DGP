#pragma once

#include <concepts>

#include "GameSubsystem.h"
#include "System.h"

namespace TUK::Framework
{
	class TimeSubsystem;

	class Game : public System
	{
		friend class Engine;

	public:
		explicit Game();
		~Game() override;

		template <std::derived_from<GameSubsystem> TSubsystem>
		TSubsystem* AddSubsystem();

		template <std::derived_from<GameSubsystem> TSubsystem>
		[[nodiscard]] TSubsystem& GetSubsystem();

		template <std::derived_from<GameSubsystem> TSubsystem>
		[[nodiscard]] const TSubsystem& GetSubsystem() const;

		[[nodiscard]] auto GetSubsystems();
		[[nodiscard]] const auto GetSubsystems() const;

	private:
		void Begin();
		void Tick();
		void End();
	};

	template <std::derived_from<GameSubsystem> TSubsystem>
	TSubsystem* Game::AddSubsystem()
	{
		return System::AddSubsystem<TSubsystem>();
	}

	template <std::derived_from<GameSubsystem> TSubsystem>
	TSubsystem& Game::GetSubsystem()
	{
		return System::GetSubsystem<TSubsystem>();
	}

	template <std::derived_from<GameSubsystem> TSubsystem>
	const TSubsystem& Game::GetSubsystem() const
	{
		return System::GetSubsystem<TSubsystem>();
	}

	inline auto Game::GetSubsystems()
	{
		return System::GetSubsystems<GameSubsystem>();
	}

	inline const auto Game::GetSubsystems() const
	{
		return System::GetSubsystems<GameSubsystem>();
	}
}