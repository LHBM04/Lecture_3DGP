#include "Precompiled.hpp"
#include "World.hpp"

#include <iostream>

#include "../Graphics/Camera.hpp"
#include "../Graphics/GPUCommandBuffer.hpp"

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
		// Camera는 5차 과제에서 독립 객체로 관리합니다.
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

	void World::PreRender(GPUCommandBuffer& commandBuffer)
	{
		if (!isEntered)
		{
			return;
		}
		OnPreRender(commandBuffer);
		if (!isEntered)
		{
			return;
		}
		const usize count = entities.size();
		for (usize index = 0; index < count; ++index)
		{
			if (!isEntered)
			{
				return;
			}
			Entity* entity = entities[index].get();
			entity->PreRender(commandBuffer);
		}
	}

	void World::Render(GPUCommandBuffer& commandBuffer)
	{
		if (!isEntered)
		{
			return;
		}
		const Camera* camera = GetMainCamera();
		if (!camera)
		{
			return;
		}
		commandBuffer.SetCamera(*camera);

		OnRender(commandBuffer);
		if (!isEntered)
		{
			return;
		}
		const usize count = entities.size();
		for (usize index = 0; index < count; ++index)
		{
			if (!isEntered)
			{
				return;
			}
			Entity* entity = entities[index].get();
			entity->Render(commandBuffer);
		}
	}

	void World::PostRender(GPUCommandBuffer& commandBuffer)
	{
		if (!isEntered)
		{
			return;
		}
		OnPostRender(commandBuffer);
		if (!isEntered)
		{
			return;
		}
		const usize count = entities.size();
		for (usize index = 0; index < count; ++index)
		{
			if (!isEntered)
			{
				return;
			}
			Entity* entity = entities[index].get();
			entity->PostRender(commandBuffer);
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

	void World::OnPreRender(GPUCommandBuffer&)
	{
	}

	void World::OnRender(GPUCommandBuffer&)
	{
	}

	void World::OnPostRender(GPUCommandBuffer&)
	{
	}

	void World::OnExit()
	{
	}
}
