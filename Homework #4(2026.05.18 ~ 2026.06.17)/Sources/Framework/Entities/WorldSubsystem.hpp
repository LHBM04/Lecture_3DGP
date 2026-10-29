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
	public:
		WorldSubsystem() noexcept;
		~WorldSubsystem() noexcept override;
		World& AddWorld(std::unique_ptr<World> world);

		void PreRender(GPUCommandBuffer& renderer);
		void Render(GPUCommandBuffer& renderer);
		void PostRender(GPUCommandBuffer& renderer);

	protected:
		SystemStatus OnStartup() override;
		SystemStatus OnEarlyUpdate() override;
		SystemStatus OnFixedUpdate() override;
		SystemStatus OnUpdate() override;
		SystemStatus OnLateUpdate() override;
		void OnShutdown() override;

	private:
		std::vector<std::unique_ptr<World>> worlds;
		World* currentWorld = nullptr;
	};
}
