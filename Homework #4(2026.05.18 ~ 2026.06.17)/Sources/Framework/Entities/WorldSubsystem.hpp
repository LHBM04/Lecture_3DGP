#pragma once

#include <memory>
#include <vector>

#include "../Core/AppSubsystem.hpp"

namespace TUK::Framework
{
	class GPUCommandBuffer;
	class World;

	class WorldSubsystem : public AppSubsystem
	{
		friend class System;

	public:
		~WorldSubsystem() noexcept override;
		World& AddWorld(std::unique_ptr<World> world);
		void ExitWorld();

		void PreRender(GPUCommandBuffer& commandBuffer);
		void Render(GPUCommandBuffer& commandBuffer);
		void PostRender(GPUCommandBuffer& commandBuffer);

	protected:
		SystemStatus OnStartup() override;
		SystemStatus OnEarlyUpdate() override;
		SystemStatus OnFixedUpdate() override;
		SystemStatus OnUpdate() override;
		SystemStatus OnLateUpdate() override;
		void OnShutdown() override;

	private:
		WorldSubsystem() noexcept;

		std::vector<std::unique_ptr<World>> worlds;
		World* currentWorld = nullptr;
	};
}
