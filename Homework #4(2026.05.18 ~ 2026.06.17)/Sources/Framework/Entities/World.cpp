#include "Precompiled.hpp"
#include "Framework/Entities/World.hpp"

#include "Framework/Entities/Entity.hpp"

namespace TUK::Framework
{
	World::World() noexcept
		: isEntered(false)
	{
	}

	World::~World() noexcept
	{
	}

	void World::Enter()
	{
		if (isEntered)
		{
			return;
		}

		isEntered = true;
		OnEnter();
	}

	Entity& World::CreateEntity()
	{
		auto entity = std::make_unique<Entity>();
		entity->world = this;
		Entity& result = *entity;
		entities.push_back(std::move(entity));
		return result;
	}

	void World::ClearEntities() noexcept
	{
		entities.clear();
	}

	void World::Exit()
	{
		if (!isEntered)
		{
			return;
		}

		isEntered = false;
		OnExit();
	}

	void World::FixedUpdate()
	{
		OnFixedUpdate();

		for (const auto& entity : entities)
		{
			entity->FixedUpdate();
		}
	}

	void World::Update()
	{
		OnUpdate();

		for (const auto& entity : entities)
		{
			entity->Update();
		}
	}

	void World::LateUpdate()
	{
		OnLateUpdate();

		for (const auto& entity : entities)
		{
			entity->LateUpdate();
		}
	}

	void World::Render(CommandBuffer& renderer)
	{
		if (!isEntered) return;
		OnRender(renderer);
		if (!isEntered) return;
		const std::size_t count = entities.size();
		for (std::size_t index = 0; index < count; ++index)
		{
			if (!isEntered) return;
			Entity* entity = entities[index].get();
			entity->Render(renderer);
		}
	}

	void World::OnEnter()
	{
	}

	void World::OnFixedUpdate()
	{
	}

	void World::OnUpdate()
	{
	}

	void World::OnLateUpdate()
	{
	}

	void World::OnRender(CommandBuffer&)
	{
	}

	void World::OnExit()
	{
	}
}
