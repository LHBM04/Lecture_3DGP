#include "Precompiled.hpp"
#include "WorldSubsystem.hpp"

#include "World.hpp"

#include "../Utilities/Logger.hpp"

namespace TUK::Framework
{
	WorldSubsystem::~WorldSubsystem() noexcept
	{
		worlds.clear();
	}

	World* WorldSubsystem::AddWorld(std::unique_ptr<World> world)
	{
		ASSERT_FMT(world, "world가 nullptr입니다.");
		ASSERT_FMT(!currentWorld, "currentWorld가 이미 설정되어 있습니다.");

		World* result = world.get();
		worlds.push_back(std::move(world));
		return result;
	}

	void WorldSubsystem::ExitWorld()
	{
		if (!currentWorld)
		{
			return;
		}

		World* exitingWorld = currentWorld;
		currentWorld = nullptr;

		exitingWorld->Exit();
	}

	void WorldSubsystem::PreRender(GPUCommandBuffer& commandBuffer)
	{
		if (currentWorld)
		{
			currentWorld->PreRender(commandBuffer);
		}
	}

	void WorldSubsystem::Render(GPUCommandBuffer& commandBuffer)
	{
		if (currentWorld)
		{
			currentWorld->Render(commandBuffer);
		}
	}

	void WorldSubsystem::PostRender(GPUCommandBuffer& commandBuffer)
	{
		if (currentWorld)
		{
			currentWorld->PostRender(commandBuffer);
		}
	}

	SystemStatus WorldSubsystem::OnStartup()
	{
		if (worlds.empty())
		{
			return SystemStatus::FAILURE;
		}

		currentWorld = worlds.front().get();
		return SystemStatus::CONTINUE;
	}

	SystemStatus WorldSubsystem::OnEarlyUpdate()
	{
		if (!currentWorld)
		{
			return SystemStatus::FAILURE;
		}

		currentWorld->Enter();

		if (currentWorld)
		{
			currentWorld->EarlyUpdate();
		}
		return SystemStatus::CONTINUE;
	}

	SystemStatus WorldSubsystem::OnFixedUpdate()
	{
		if (currentWorld)
		{
			currentWorld->FixedUpdate();
		}
		return SystemStatus::CONTINUE;
	}

	SystemStatus WorldSubsystem::OnUpdate()
	{
		if (currentWorld)
		{
			currentWorld->Update();
		}
		return SystemStatus::CONTINUE;
	}

	SystemStatus WorldSubsystem::OnLateUpdate()
	{
		if (currentWorld)
		{
			currentWorld->LateUpdate();
		}
		return currentWorld ? SystemStatus::CONTINUE : SystemStatus::SUCCESS;
	}

	void WorldSubsystem::OnShutdown()
	{
		ExitWorld();
		worlds.clear();
	}
}
