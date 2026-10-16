#pragma once

#include <memory>
#include <vector>

#include "../Core/GameSubsystem.hpp"

namespace TUK::Framework
{
	class World;

	class WorldSubsystem : public GameSubsystem
	{
	public:
		WorldSubsystem() noexcept;
		~WorldSubsystem() noexcept override;
		World& AddWorld(std::unique_ptr<World> world);

	protected:
		void OnStartup() override;
		void OnShutdown() override;

		void FixedUpdate() override;
		void Update() override;
		void LateUpdate() override;
		void Render(CommandBuffer& renderer) override;

	private:
		std::vector<std::unique_ptr<World>> worlds;
		World* currentWorld = nullptr;
	};
}
