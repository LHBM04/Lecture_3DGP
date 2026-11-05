#pragma once

#include <concepts>

#include "AppSubsystem.hpp"
#include "System.hpp"
#include "SystemStatus.hpp"

namespace TUK::Framework
{
	class App : public System
	{
	public:
		SystemStatus Startup() override;
		SystemStatus Routine() override;
		void Shutdown() override;
		
		template <std::derived_from<AppSubsystem> TSubsystem>
		TSubsystem* AddSubsystem();

		template <std::derived_from<AppSubsystem> TSubsystem>
		[[nodiscard]] TSubsystem* GetSubsystem() noexcept;

		template <std::derived_from<AppSubsystem> TSubsystem>
		[[nodiscard]] const TSubsystem* GetSubsystem() const noexcept;

		[[nodiscard]] auto GetSubsystems() noexcept;
		[[nodiscard]] auto GetSubsystems() const noexcept;

		[[nodiscard]] static App& GetInstance() noexcept;

	private:
		SystemStatus EarlyUpdate();
		SystemStatus FixedUpdate();
		SystemStatus Update();
		SystemStatus LateUpdate();

		inline static App* instance = nullptr;
	};

	template <std::derived_from<AppSubsystem> TSubsystem>
	TSubsystem* App::AddSubsystem()
	{
		return System::AddSubsystem<TSubsystem>();
	}

	template <std::derived_from<AppSubsystem> TSubsystem>
	TSubsystem* App::GetSubsystem() noexcept
	{
		return System::GetSubsystem<TSubsystem>();
	}

	template <std::derived_from<AppSubsystem> TSubsystem>
	const TSubsystem* App::GetSubsystem() const noexcept
	{
		return System::GetSubsystem<TSubsystem>();
	}

	inline auto App::GetSubsystems() noexcept
	{
		return System::GetSubsystems<AppSubsystem>();
	}

	inline auto App::GetSubsystems() const noexcept
	{
		return System::GetSubsystems<AppSubsystem>();
	}

	inline App& App::GetInstance() noexcept
	{
		return *instance;
	}
}
