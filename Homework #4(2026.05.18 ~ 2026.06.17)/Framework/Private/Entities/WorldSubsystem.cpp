#include "Precompiled.hpp"
#include "Framework/Entities/WorldSubsystem.hpp"

#include "Framework/Entities/World.hpp"

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

	void WorldSubsystem::OnStartup()
	{
		if (worlds.empty()) return;
		currentWorld = worlds.front().get();
		currentWorld->Enter();
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
		if (currentWorld) currentWorld->Exit();
		currentWorld = nullptr;
		worlds.clear();
	}

	void WorldSubsystem::FixedUpdate()
	{
		if (!currentWorld) return;
		currentWorld->FixedUpdate();
	}

	void WorldSubsystem::Update()
	{
		if (!currentWorld) return;
		currentWorld->Update();
	}

	void WorldSubsystem::LateUpdate()
	{
		if (!currentWorld) return;
		currentWorld->LateUpdate();
	}

	void WorldSubsystem::Render(CommandBuffer& renderer)
	{
		if (!currentWorld) return;
		currentWorld->Render(renderer);
	}
}
