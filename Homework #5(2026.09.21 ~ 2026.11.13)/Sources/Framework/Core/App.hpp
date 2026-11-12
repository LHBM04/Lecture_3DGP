#pragma once

#include "Common.hpp"

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
		void RemoveSubsystem();

		template <std::derived_from<AppSubsystem> TSubsystem>
		[[nodiscard]] TSubsystem* GetSubsystem() noexcept;

		template <std::derived_from<AppSubsystem> TSubsystem>
		[[nodiscard]] const TSubsystem* GetSubsystem() const noexcept;

		template <std::derived_from<AppSubsystem> TSubsystem>
		Delegate<AppSubsystem*>::ListenerId OnSubsystemAdded(Action<TSubsystem*> callback);

		template <std::derived_from<AppSubsystem> TSubsystem, class TObject>
		Delegate<AppSubsystem*>::ListenerId OnSubsystemAdded(TObject* object, void (TObject::*method)(TSubsystem*));

		template <std::derived_from<AppSubsystem> TSubsystem>
		Delegate<AppSubsystem*>::ListenerId OnSubsystemRemoved(Action<TSubsystem*> callback);

		template <std::derived_from<AppSubsystem> TSubsystem, class TObject>
		Delegate<AppSubsystem*>::ListenerId OnSubsystemRemoved(TObject* object, void (TObject::*method)(TSubsystem*));

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
	void App::RemoveSubsystem()
	{
		System::RemoveSubsystem<TSubsystem>();
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

	template <std::derived_from<AppSubsystem> TSubsystem>
	Delegate<AppSubsystem*>::ListenerId App::OnSubsystemAdded(Action<TSubsystem*> callback)
	{
		return System::OnSubsystemAdded<TSubsystem>(std::move(callback));
	}

	template <std::derived_from<AppSubsystem> TSubsystem, class TObject>
	Delegate<AppSubsystem*>::ListenerId App::OnSubsystemAdded(TObject* object, void (TObject::*method)(TSubsystem*))
	{
		return System::OnSubsystemAdded<TSubsystem>(object, method);
	}

	template <std::derived_from<AppSubsystem> TSubsystem>
	Delegate<AppSubsystem*>::ListenerId App::OnSubsystemRemoved(Action<TSubsystem*> callback)
	{
		return System::OnSubsystemRemoved<TSubsystem>(std::move(callback));
	}

	template <std::derived_from<AppSubsystem> TSubsystem, class TObject>
	Delegate<AppSubsystem*>::ListenerId App::OnSubsystemRemoved(TObject* object, void (TObject::*method)(TSubsystem*))
	{
		return System::OnSubsystemRemoved<TSubsystem>(object, method);
	}

	inline App& App::GetInstance() noexcept
	{
		ASSERT_FMT(instance, "App 인스턴스가 초기화되지 않았습니다.");

		return *instance;
	}
}
