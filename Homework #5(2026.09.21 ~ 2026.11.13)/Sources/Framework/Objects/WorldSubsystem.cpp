#include "Precompiled.hpp"
#include "WorldSubsystem.hpp"

#include <algorithm>

#include "World.hpp"

#include "../Core/Common.hpp"

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

	bool WorldSubsystem::RequestWorldChange(World* targetWorld) noexcept
	{
		if (!targetWorld || targetWorld == currentWorld)
		{
			return false;
		}

		const bool isRegistered = std::ranges::any_of(worlds, [targetWorld](const std::unique_ptr<World>& world)
		{
			return world.get() == targetWorld;
		});
		if (!isRegistered)
		{
			return false;
		}

		nextWorld = targetWorld;
		return true;
	}

	void WorldSubsystem::ExitWorld()
	{
		nextWorld = nullptr;
		ExitCurrentWorld();
		nextWorld = nullptr;
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
		if (nextWorld)
		{
			if (nextWorld != currentWorld)
			{
				ExitCurrentWorld();
			}
			currentWorld = std::exchange(nextWorld, nullptr);
		}

		if (!currentWorld)
		{
			return SystemStatus::SUCCESS;
		}

		if (!currentWorld->IsEntered())
		{
			World* enteringWorld = currentWorld;
			enteringWorld->Enter();
			if (currentWorld == enteringWorld)
			{
				onEnterWorld.Broadcast(enteringWorld);
				NotifyIfWorldExited(enteringWorld);
			}
		}

		if (currentWorld)
		{
			World* updatingWorld = currentWorld;
			updatingWorld->EarlyUpdate();
			NotifyIfWorldExited(updatingWorld);
		}
		return SystemStatus::CONTINUE;
	}

	SystemStatus WorldSubsystem::OnFixedUpdate()
	{
		if (currentWorld)
		{
			World* updatingWorld = currentWorld;
			updatingWorld->FixedUpdate();
			NotifyIfWorldExited(updatingWorld);
		}
		return SystemStatus::CONTINUE;
	}

	SystemStatus WorldSubsystem::OnUpdate()
	{
		if (currentWorld)
		{
			World* updatingWorld = currentWorld;
			updatingWorld->Update();
			NotifyIfWorldExited(updatingWorld);
		}
		return SystemStatus::CONTINUE;
	}

	SystemStatus WorldSubsystem::OnLateUpdate()
	{
		if (currentWorld)
		{
			World* updatingWorld = currentWorld;
			updatingWorld->LateUpdate();
			NotifyIfWorldExited(updatingWorld);
		}
		return currentWorld || nextWorld ? SystemStatus::CONTINUE : SystemStatus::SUCCESS;
	}

	void WorldSubsystem::OnShutdown()
	{
		ExitWorld();
		onEnterWorld.Clear();
		onExitWorld.Clear();
		worlds.clear();
	}

	void WorldSubsystem::ExitCurrentWorld()
	{
		World* exitingWorld = std::exchange(currentWorld, nullptr);
		if (exitingWorld && exitingWorld->IsEntered())
		{
			exitingWorld->Exit();
			onExitWorld.Broadcast(exitingWorld);
		}
	}

	void WorldSubsystem::NotifyIfWorldExited(World* world)
	{
		if (currentWorld == world && !world->IsEntered())
		{
			currentWorld = nullptr;
			onExitWorld.Broadcast(world);
		}
	}

}
