#include "Precompiled.hpp"
#include "World.hpp"

#include "../Graphics/Camera.hpp"

#include "Entity.hpp"

namespace TUK::Framework
{
	World::World() noexcept
		: isEntered(false)
	{
	}

	World::~World() noexcept
	{
		// Camera 소멸자가 등록을 해제할 때 카메라 목록이 살아 있어야 합니다.
		entities.clear();
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
		return CreateEntity("New Entity", "Untagged");
	}

	Entity& World::CreateEntity(std::string_view name, std::string_view tag)
	{
		auto entity = std::make_unique<Entity>(name, tag, *this);
		Entity& result = *entity;
		entities.push_back(std::move(entity));
		return result;
	}

	void World::AddCamera(Camera& camera)
	{
		assert(&camera.GetOwner().GetWorld() == this);
		if (std::ranges::find(cameras, &camera) == cameras.end())
		{
			cameras.emplace_back(&camera);
		}
	}

	void World::RemoveCamera(const Camera& camera)
	{
		std::erase(cameras, &camera);
	}

	Camera* World::GetMainCamera() noexcept
	{
		return cameras.empty() ? nullptr : cameras.front();
	}

	const Camera* World::GetMainCamera() const noexcept
	{
		return cameras.empty() ? nullptr : cameras.front();
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

	void World::PreRender(D3DCommandBuffer& renderer)
	{
		if (!isEntered)
		{
			return;
		}
		OnPreRender(renderer);
		if (!isEntered)
		{
			return;
		}
		const std::size_t count = entities.size();
		for (std::size_t index = 0; index < count; ++index)
		{
			if (!isEntered)
			{
				return;
			}
			Entity* entity = entities[index].get();
			entity->PreRender(renderer);
		}
	}

	void World::Render(D3DCommandBuffer& renderer)
	{
		if (!isEntered)
		{
			return;
		}
		OnRender(renderer);
		if (!isEntered)
		{
			return;
		}
		const std::size_t count = entities.size();
		for (std::size_t index = 0; index < count; ++index)
		{
			if (!isEntered)
			{
				return;
			}
			Entity* entity = entities[index].get();
			entity->Render(renderer);
		}
	}

	void World::PostRender(D3DCommandBuffer& renderer)
	{
		if (!isEntered)
		{
			return;
		}
		OnPostRender(renderer);
		if (!isEntered)
		{
			return;
		}
		const std::size_t count = entities.size();
		for (std::size_t index = 0; index < count; ++index)
		{
			if (!isEntered)
			{
				return;
			}
			Entity* entity = entities[index].get();
			entity->PostRender(renderer);
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

	void World::OnPreRender(D3DCommandBuffer&)
	{
	}

	void World::OnRender(D3DCommandBuffer&)
	{
	}

	void World::OnPostRender(D3DCommandBuffer&)
	{
	}

	void World::OnExit()
	{
	}
}
