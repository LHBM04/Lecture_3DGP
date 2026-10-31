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
		~App() noexcept override = default;

		SystemStatus Startup() override;
		SystemStatus Routine() override;
		void Shutdown() override;
		
		template <std::derived_from<AppSubsystem> TSubsystem>
		TSubsystem* AddSubsystem();

		template <std::derived_from<AppSubsystem> TSubsystem>
		[[nodiscard]] TSubsystem* GetSubsystem();

		template <std::derived_from<AppSubsystem> TSubsystem>
		[[nodiscard]] const TSubsystem* GetSubsystem() const;

		[[nodiscard]] auto GetSubsystems();
		[[nodiscard]] const auto GetSubsystems() const;

		[[nodiscard]] static App& GetInstance();

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
	TSubsystem* App::GetSubsystem()
	{
		return System::GetSubsystem<TSubsystem>();
	}

	template <std::derived_from<AppSubsystem> TSubsystem>
	const TSubsystem* App::GetSubsystem() const
	{
		return System::GetSubsystem<TSubsystem>();
	}

	inline auto App::GetSubsystems()
	{
		return System::GetSubsystems<AppSubsystem>();
	}

	inline const auto App::GetSubsystems() const
	{
		return System::GetSubsystems<AppSubsystem>();
	}

	inline App& App::GetInstance()
	{
		return *instance;
	}
}
