#pragma once

#include <concepts>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include "../Core/AppSubsystem.hpp"
#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class GPUCommandBuffer;
	class World;

	class WorldSubsystem : public AppSubsystem
	{
	public:
		using WorldCallback = std::function<void(World*)>;

		WorldSubsystem() noexcept;
		~WorldSubsystem() noexcept override;

		World* AddWorld(std::unique_ptr<World> world);
		void ExitWorld();

		template <class TListener, class TArg>
			requires std::invocable<void (TListener::*)(TArg), TListener*, World*>
		void AddEnterListener(TListener* listener, void (TListener::* callback)(TArg));

		template <class TListener, class TArg>
			requires std::invocable<void (TListener::*)(TArg) const, const TListener*, World*>
		void AddEnterListener(const TListener* listener, void (TListener::* callback)(TArg) const);

		template <class TListener, class TArg>
			requires std::invocable<void (TListener::*)(TArg), TListener*, World*>
		void AddExitListener(TListener* listener, void (TListener::* callback)(TArg));

		template <class TListener, class TArg>
			requires std::invocable<void (TListener::*)(TArg) const, const TListener*, World*>
		void AddExitListener(const TListener* listener, void (TListener::* callback)(TArg) const);

		[[nodiscard]] World* GetCurrentWorld() noexcept;
		[[nodiscard]] const World* GetCurrentWorld() const noexcept;

		void PreRender(GPUCommandBuffer& commandBuffer);
		void Render(GPUCommandBuffer& commandBuffer);
		void PostRender(GPUCommandBuffer& commandBuffer);

		SystemStatus OnStartup() override;
		SystemStatus OnEarlyUpdate() override;
		SystemStatus OnFixedUpdate() override;
		SystemStatus OnUpdate() override;
		SystemStatus OnLateUpdate() override;
		void OnShutdown() override;

	private:
		void AddEnterListener(WorldCallback callback);
		void AddExitListener(WorldCallback callback);
		void NotifyWorldEntered(World* world);
		void NotifyWorldExited(World* world);

		std::vector<std::unique_ptr<World>> worlds;
		World* currentWorld = nullptr;

		std::vector<WorldCallback> enterListeners;
		std::vector<WorldCallback> exitListeners;
	};

	inline WorldSubsystem::WorldSubsystem() noexcept
		: AppSubsystem(20)
		, worlds()
	{
	}

	template <class TListener, class TArg>
		requires std::invocable<void (TListener::*)(TArg), TListener*, World*>
	void WorldSubsystem::AddEnterListener(TListener* listener, void (TListener::* callback)(TArg))
	{
		ASSERT_FMT(listener, "월드 진입 이벤트 수신 객체가 nullptr입니다.");
		ASSERT_FMT(callback, "월드 진입 이벤트 콜백이 nullptr입니다.");

		AddEnterListener(
			[listener, callback](World* world)
			{
				(listener->*callback)(world);
			});
	}

	template <class TListener, class TArg>
		requires std::invocable<void (TListener::*)(TArg) const, const TListener*, World*>
	void WorldSubsystem::AddEnterListener(const TListener* listener, void (TListener::* callback)(TArg) const)
	{
		ASSERT_FMT(listener, "월드 진입 이벤트 수신 객체가 nullptr입니다.");
		ASSERT_FMT(callback, "월드 진입 이벤트 콜백이 nullptr입니다.");

		AddEnterListener(
			[listener, callback](World* world)
			{
				(listener->*callback)(world);
			});
	}

	template <class TListener, class TArg>
		requires std::invocable<void (TListener::*)(TArg), TListener*, World*>
	void WorldSubsystem::AddExitListener(TListener* listener, void (TListener::* callback)(TArg))
	{
		ASSERT_FMT(listener, "월드 탈출 이벤트 수신 객체가 nullptr입니다.");
		ASSERT_FMT(callback, "월드 탈출 이벤트 콜백이 nullptr입니다.");

		AddExitListener(
			[listener, callback](World* world)
			{
				(listener->*callback)(world);
			});
	}

	template <class TListener, class TArg>
		requires std::invocable<void (TListener::*)(TArg) const, const TListener*, World*>
	void WorldSubsystem::AddExitListener(const TListener* listener, void (TListener::* callback)(TArg) const)
	{
		ASSERT_FMT(listener, "월드 탈출 이벤트 수신 객체가 nullptr입니다.");
		ASSERT_FMT(callback, "월드 탈출 이벤트 콜백이 nullptr입니다.");

		AddExitListener(
			[listener, callback](World* world)
			{
				(listener->*callback)(world);
			});
	}

	inline World* WorldSubsystem::GetCurrentWorld() noexcept
	{
		return currentWorld;
	}

	inline const World* WorldSubsystem::GetCurrentWorld() const noexcept
	{
		return currentWorld;
	}
}
