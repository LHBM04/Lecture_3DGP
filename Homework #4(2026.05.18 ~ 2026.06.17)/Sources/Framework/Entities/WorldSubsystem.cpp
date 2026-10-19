#include "Precompiled.hpp"
#include "WorldSubsystem.hpp"

#include "World.hpp"

namespace TUK::Framework
{
	WorldSubsystem::WorldSubsystem() noexcept
		: GameSubsystem(20)
		, worlds()
	{
	}

	WorldSubsystem::~WorldSubsystem() noexcept
	{
		worlds.clear();
	}

	SystemStatus WorldSubsystem::OnStartup()
	{
		if (worlds.empty())
		{
			return SystemStatus::Failure;
		}

		currentWorld = worlds.front().get();
		currentWorld->Enter();
		return SystemStatus::Continue;
	}

	World& WorldSubsystem::AddWorld(std::unique_ptr<World> world)
	{
		if (!world) throw std::invalid_argument("World must not be null.");
		if (currentWorld) throw std::logic_error("Register worlds before startup.");
		World& result = *world;
		worlds.push_back(std::move(world));
		return result;
	}

	void WorldSubsystem::OnShutdown()
	{
		currentWorld = nullptr;
		worlds.clear();
	}

	SystemStatus WorldSubsystem::OnFixedUpdate()
	{
		if (!currentWorld)
		{
			return SystemStatus::Failure;
		}

		currentWorld->FixedUpdate();
	}

	SystemStatus WorldSubsystem::OnUpdate()
	{
		if (!currentWorld)
		{
			return SystemStatus::Failure;
		}

		currentWorld->Update();
	}

	SystemStatus WorldSubsystem::OnLateUpdate()
	{
		if (!currentWorld)
		{
			return SystemStatus::Failure;
		}

		currentWorld->LateUpdate();
	}
}
