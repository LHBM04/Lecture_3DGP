#pragma once

#include <cstdint>

#include "EngineSubsystem.hpp"
#include "System.hpp"

#define DECLARE_ENGINE_INSTANCE() \
	friend ::TUK::Framework::Engine* ::TUK::Framework::CreateEngineInstance()

#define DEFINE_ENGINE_INSTANCE(Type) \
	namespace TUK::Framework \
	{ \
		Engine* CreateEngineInstance() \
		{ \
			static_assert(std::derived_from<Type, Engine>); \
			return new Type(); \
		} \
	}

namespace TUK::Framework
{
	class App;

	class Engine : public System
	{
	public:
		~Engine() noexcept override;

		template <std::derived_from<EngineSubsystem> TSubsystem>
		TSubsystem* AddSubsystem();

		template <std::derived_from<EngineSubsystem> TSubsystem>
		[[nodiscard]] TSubsystem* GetSubsystem();

		template <std::derived_from<EngineSubsystem> TSubsystem>
		[[nodiscard]] const TSubsystem* GetSubsystem() const;

		[[nodiscard]] auto GetSubsystems();
		[[nodiscard]] const auto GetSubsystems() const;

		/** Overloaded for Engine */
		[[nodiscard]] static Engine* GetInstance();

		std::int32_t Run(App& app);

		SystemStatus Routine() override;

		/** 엔진 라이프 사이클 */
		SystemStatus PreTick();
		SystemStatus Tick();
		SystemStatus PostTick();

	protected:
		Engine() noexcept;

	private:
		static Engine* instance;

		App* app;
	};

	/** 엔진 인스턴스 팩토리 */
	extern Engine* CreateEngineInstance();

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
}
