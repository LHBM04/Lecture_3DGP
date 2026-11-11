#pragma once

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
		WorldSubsystem() noexcept;
		~WorldSubsystem() noexcept override;

		World* AddWorld(std::unique_ptr<World> world);
		void ExitWorld();

		[[nodiscard]] bool RequestWorldChange(World* targetWorld) noexcept;

		[[nodiscard]] Delegate<World*>& OnEnterWorld() noexcept;
		[[nodiscard]] Delegate<World*>& OnExitWorld() noexcept;

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
		void ExitCurrentWorld();
		void NotifyIfWorldExited(World* world);

		std::vector<std::unique_ptr<World>> worlds;
		World* currentWorld = nullptr;
		World* nextWorld = nullptr;

		Delegate<World*> onEnterWorld;
		Delegate<World*> onExitWorld;
	};

	inline WorldSubsystem::WorldSubsystem() noexcept
		: AppSubsystem(20)
		, worlds()
	{
	}

	inline Delegate<World*>& WorldSubsystem::OnEnterWorld() noexcept
	{
		return onEnterWorld;
	}

	inline Delegate<World*>& WorldSubsystem::OnExitWorld() noexcept
	{
		return onExitWorld;
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
