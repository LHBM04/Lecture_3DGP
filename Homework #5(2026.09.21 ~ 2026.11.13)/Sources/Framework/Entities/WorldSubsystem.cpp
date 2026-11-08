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

		if (exitingWorld->IsEntered())
		{
			exitingWorld->Exit();
			NotifyWorldExited(exitingWorld);
		}
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

		if (!currentWorld->IsEntered())
		{
			World* enteringWorld = currentWorld;
			enteringWorld->Enter();
			NotifyWorldEntered(enteringWorld);

			if (!enteringWorld->IsEntered())
			{
				NotifyWorldExited(enteringWorld);
				if (currentWorld == enteringWorld)
				{
					currentWorld = nullptr;
				}
				return SystemStatus::CONTINUE;
			}
		}

		if (currentWorld)
		{
			World* updatingWorld = currentWorld;
			updatingWorld->EarlyUpdate();
			if (!updatingWorld->IsEntered())
			{
				NotifyWorldExited(updatingWorld);
				if (currentWorld == updatingWorld)
				{
					currentWorld = nullptr;
				}
			}
		}
		return SystemStatus::CONTINUE;
	}

	SystemStatus WorldSubsystem::OnFixedUpdate()
	{
		if (currentWorld)
		{
			World* updatingWorld = currentWorld;
			updatingWorld->FixedUpdate();
			if (!updatingWorld->IsEntered())
			{
				NotifyWorldExited(updatingWorld);
				if (currentWorld == updatingWorld)
				{
					currentWorld = nullptr;
				}
			}
		}
		return SystemStatus::CONTINUE;
	}

	SystemStatus WorldSubsystem::OnUpdate()
	{
		if (currentWorld)
		{
			World* updatingWorld = currentWorld;
			updatingWorld->Update();
			if (!updatingWorld->IsEntered())
			{
				NotifyWorldExited(updatingWorld);
				if (currentWorld == updatingWorld)
				{
					currentWorld = nullptr;
				}
			}
		}
		return SystemStatus::CONTINUE;
	}

	SystemStatus WorldSubsystem::OnLateUpdate()
	{
		if (currentWorld)
		{
			World* updatingWorld = currentWorld;
			updatingWorld->LateUpdate();
			if (!updatingWorld->IsEntered())
			{
				NotifyWorldExited(updatingWorld);
				if (currentWorld == updatingWorld)
				{
					currentWorld = nullptr;
				}
			}
		}
		return currentWorld ? SystemStatus::CONTINUE : SystemStatus::SUCCESS;
	}

	void WorldSubsystem::OnShutdown()
	{
		ExitWorld();
		enterListeners.clear();
		exitListeners.clear();
		worlds.clear();
	}

	void WorldSubsystem::AddEnterListener(WorldCallback callback)
	{
		enterListeners.push_back(std::move(callback));
	}

	void WorldSubsystem::AddExitListener(WorldCallback callback)
	{
		exitListeners.push_back(std::move(callback));
	}

	void WorldSubsystem::NotifyWorldEntered(World* world)
	{
		for (const WorldCallback& listener : enterListeners)
		{
			listener(world);
		}
	}

	void WorldSubsystem::NotifyWorldExited(World* world)
	{
		for (const WorldCallback& listener : exitListeners)
		{
			listener(world);
		}
	}
}
