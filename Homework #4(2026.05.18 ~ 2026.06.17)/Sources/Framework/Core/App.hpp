#pragma once

#include <concepts>

#include "../Platform/DisplayCreateInfo.hpp"

#include "AppSubsystem.hpp"
#include "System.hpp"

#define DECLARE_APP_INSTANCE() \
	friend ::TUK::Framework::App* ::TUK::Framework::CreateAppInstance()

#define DEFINE_APP_INSTANCE(AppType) \
	namespace TUK::Framework \
	{ \
		App* CreateAppInstance() \
		{ \
			static_assert(std::derived_from<AppType, App>); \
			return new AppType(); \
		} \
	}

namespace TUK::Framework
{
	class App : public System
	{
		friend class Engine;

	public:
		~App() noexcept override;

		template <std::derived_from<AppSubsystem> TSubsystem>
		TSubsystem* AddSubsystem();

		template <std::derived_from<AppSubsystem> TSubsystem>
		[[nodiscard]] TSubsystem* GetSubsystem();

		template <std::derived_from<AppSubsystem> TSubsystem>
		[[nodiscard]] const TSubsystem* GetSubsystem() const;

		[[nodiscard]] auto GetSubsystems();
		[[nodiscard]] const auto GetSubsystems() const;

		SystemStatus Routine() override;

		/** Overloaded for App */
		[[nodiscard]] static App* GetInstance();

	protected:
		App() noexcept;

		/** 앱 라이프 사이클 */
		SystemStatus EarlyUpdate();
		SystemStatus FixedUpdate();
		SystemStatus Update();
		SystemStatus LateUpdate();

	private:
		static App* instance;
	};

	/** 앱 인스턴스 팩토리 */
	extern App* CreateAppInstance();

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
}
