#pragma once

#include <concepts>

#include "../Utilities/Logger.hpp"

#include "AppSubsystem.hpp"
#include "System.hpp"
#include "SystemStatus.hpp"

namespace TUK::Framework
{
	class App : public System<AppSubsystem>
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

	inline App& App::GetInstance() noexcept
	{
		ASSERT_FMT(instance, "App 인스턴스가 초기화되지 않았습니다.");

		return *instance;
	}
}
