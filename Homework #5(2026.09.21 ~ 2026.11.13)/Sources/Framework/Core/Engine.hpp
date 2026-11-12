#pragma once

#include "Common.hpp"

#include "System.hpp"
#include "EngineSubsystem.hpp"

namespace TUK::Framework
{
	class App;

	class Engine : public System<EngineSubsystem>
	{
	public:
		SystemStatus Startup() override;
		SystemStatus Routine() override;
		void Shutdown() override;

		template <std::derived_from<EngineSubsystem> TSubsystem>
		TSubsystem* AddSubsystem();

		template <std::derived_from<EngineSubsystem> TSubsystem>
		void RemoveSubsystem();

		template <std::derived_from<EngineSubsystem> TSubsystem>
		[[nodiscard]] TSubsystem* GetSubsystem() noexcept;

		template <std::derived_from<EngineSubsystem> TSubsystem>
		[[nodiscard]] const TSubsystem* GetSubsystem() const noexcept;

		template <std::derived_from<EngineSubsystem> TSubsystem>
		Delegate<EngineSubsystem*>::ListenerId OnSubsystemAdded(Action<TSubsystem*> callback);

		template <std::derived_from<EngineSubsystem> TSubsystem, class TObject>
		Delegate<EngineSubsystem*>::ListenerId OnSubsystemAdded(TObject* object, void (TObject::*method)(TSubsystem*));

		template <std::derived_from<EngineSubsystem> TSubsystem>
		Delegate<EngineSubsystem*>::ListenerId OnSubsystemRemoved(Action<TSubsystem*> callback);

		template <std::derived_from<EngineSubsystem> TSubsystem, class TObject>
		Delegate<EngineSubsystem*>::ListenerId OnSubsystemRemoved(TObject* object, void (TObject::*method)(TSubsystem*));

		[[nodiscard]] i32 Run(App* target);

		[[nodiscard]] static Engine& GetInstance() noexcept;

	private:
		SystemStatus PreTick();
		SystemStatus Tick();
		SystemStatus PostTick();

		inline static Engine* instance = nullptr;

		App* app;
	};

	template <std::derived_from<EngineSubsystem> TSubsystem>
	TSubsystem* Engine::AddSubsystem()
	{
		return System::AddSubsystem<TSubsystem>();
	}

	template <std::derived_from<EngineSubsystem> TSubsystem>
	void Engine::RemoveSubsystem()
	{
		System::RemoveSubsystem<TSubsystem>();
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

	template <std::derived_from<EngineSubsystem> TSubsystem>
	Delegate<EngineSubsystem*>::ListenerId Engine::OnSubsystemAdded(Action<TSubsystem*> callback)
	{
		return System::OnSubsystemAdded<TSubsystem>(std::move(callback));
	}

	template <std::derived_from<EngineSubsystem> TSubsystem, class TObject>
	Delegate<EngineSubsystem*>::ListenerId Engine::OnSubsystemAdded(TObject* object, void (TObject::*method)(TSubsystem*))
	{
		return System::OnSubsystemAdded<TSubsystem>(object, method);
	}

	template <std::derived_from<EngineSubsystem> TSubsystem>
	Delegate<EngineSubsystem*>::ListenerId Engine::OnSubsystemRemoved(Action<TSubsystem*> callback)
	{
		return System::OnSubsystemRemoved<TSubsystem>(std::move(callback));
	}

	template <std::derived_from<EngineSubsystem> TSubsystem, class TObject>
	Delegate<EngineSubsystem*>::ListenerId Engine::OnSubsystemRemoved(TObject* object, void (TObject::*method)(TSubsystem*))
	{
		return System::OnSubsystemRemoved<TSubsystem>(object, method);
	}

	inline Engine& Engine::GetInstance() noexcept
	{
		ASSERT_FMT(instance, "Engine 인스턴스가 초기화되지 않았습니다.");

		return *instance;
	}
}
