#pragma once

#include <memory>
#include <vector>

#include "../Core/GameSubsystem.hpp"

namespace TUK::Framework
{
	class CommandBuffer;
	class World;

	class WorldSubsystem : public GameSubsystem
	{
	public:
		WorldSubsystem() noexcept;
		~WorldSubsystem() noexcept override;
		World& AddWorld(std::unique_ptr<World> world);

		void PreRender(CommandBuffer& renderer);
		void Render(CommandBuffer& renderer);
		void PostRender(CommandBuffer& renderer);

	protected:
		SystemStatus OnStartup() override;
		SystemStatus OnFixedUpdate() override;
		SystemStatus OnUpdate() override;
		SystemStatus OnLateUpdate() override;
		void OnShutdown() override;

	private:
		std::vector<std::unique_ptr<World>> worlds;
		World* currentWorld = nullptr;
	};
}
