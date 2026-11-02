#include "Precompiled.hpp"
#include "World.hpp"

#include <algorithm>
#include <ranges>

#include "../Graphics/Camera.hpp"

#include "Entity.hpp"
#include "Transform.hpp"

namespace TUK::Framework
{
	Entity& World::Instantiate()
	{
		auto entity = std::make_unique<Entity>(*this);
		Entity& reference = *entity;
		entities.push_back(std::move(entity));
		return reference;
	}

	Entity& World::Instantiate(std::string_view name)
	{
		auto entity = std::make_unique<Entity>(*this, name);
		Entity& reference = *entity;
		entities.push_back(std::move(entity));
		return reference;
	}

	Entity& World::Instantiate(std::string_view name, std::string_view tag)
	{
		auto entity = std::make_unique<Entity>(*this, name, tag);
		Entity& reference = *entity;
		entities.push_back(std::move(entity));
		return reference;
	}

	Entity& World::Instantiate(const Vector3D<f32>& position, const Quaternion<f32>& rotation)
	{
		Entity& entity = Instantiate();
		Transform* transform = entity.AddComponent<Transform>();
		transform->SetLocalPosition(position);
		transform->SetLocalRotation(rotation);
		return entity;
	}

	Entity& World::Instantiate(const Vector3D<f32>& position)
	{
		Entity& entity = Instantiate();
		Transform* transform = entity.AddComponent<Transform>();
		transform->SetLocalPosition(position);
		return entity;
	}

	Entity& World::Instantiate(const Quaternion<f32>& rotation)
	{
		Entity& entity = Instantiate();
		Transform* transform = entity.AddComponent<Transform>();
		transform->SetLocalRotation(rotation);
		return entity;
	}

	Entity& World::Instantiate(Transform* parent)
	{
		Entity& entity = Instantiate();
		Transform* transform = entity.AddComponent<Transform>();
		transform->SetParent(parent);
		return entity;
	}

	Entity& World::Instantiate(const Vector3D<f32>& position, const Quaternion<f32>& rotation, Transform* parent)
	{
		Entity& entity = Instantiate();
		Transform* transform = entity.AddComponent<Transform>();
		transform->SetParent(parent);
		transform->SetLocalPosition(position);
		transform->SetLocalRotation(rotation);
		return entity;
	}

	void World::Destroy(Entity& entity) noexcept
	{
		entity.Destroy();
	}

	void World::Destroy(Entity* entity) noexcept
	{
		if (entity)
		{
			entity->Destroy();
		}
	}

	void World::DestroyEntities()
	{
		for (const std::unique_ptr<Entity>& entity : entities)
		{
			if (!entity->IsDestroyed())
			{
				entity->RemoveDestroyedComponents();
			}
		}

		const auto destroyedCount = std::ranges::count_if(entities,
			[](const auto& entity)
			{
				return entity->IsDestroyed();
			});
		if (destroyedCount == 0)
		{
			return;
		}

		std::pmr::vector<std::unique_ptr<Entity>> destroyed;
		destroyed.reserve(static_cast<usize>(destroyedCount));

		usize remainingCount = 0;
		for (usize index = 0; index < entities.size(); ++index)
		{
			if (entities[index]->IsDestroyed())
			{
				destroyed.push_back(std::move(entities[index]));
			}
			else
			{
				if (remainingCount != index)
				{
					entities[remainingCount] = std::move(entities[index]);
				}
				++remainingCount;
			}
		}
		entities.resize(remainingCount);
	}

	void World::ClearEntities()
	{
		if (entities.empty())
		{
			cameras.clear();
			return;
		}

		std::pmr::vector<std::unique_ptr<Entity>> removing = std::move(entities);
		entities.clear();

		for (std::unique_ptr<Entity>& entity : removing | std::views::reverse)
		{
			entity->Destroy();
			entity.reset();
		}

		cameras.clear();
	}

	void World::AddCamera(Camera& camera)
	{
		if (std::ranges::find(cameras, &camera) == cameras.end())
		{
			cameras.push_back(&camera);
		}
	}

	void World::RemoveCamera(const Camera& camera)
	{
		std::erase(cameras, &camera);
	}

	Entity* World::FindEntityWithName(std::string_view name) noexcept
	{
		for (const std::unique_ptr<Entity>& entity : entities)
		{
			if (!entity->IsDestroyed() && entity->GetName() == name)
			{
				return entity.get();
			}
		}

		return nullptr;
	}

	const Entity* World::FindEntityWithName(std::string_view name) const noexcept
	{
		for (const std::unique_ptr<Entity>& entity : entities)
		{
			if (!entity->IsDestroyed() && entity->GetName() == name)
			{
				return entity.get();
			}
		}

		return nullptr;
	}

	std::vector<Entity*> World::FindEntitiesWithName(std::string_view name)
	{
		std::vector<Entity*> result;
		for (const std::unique_ptr<Entity>& entity : entities)
		{
			if (!entity->IsDestroyed() && entity->GetName() == name)
			{
				result.push_back(entity.get());
			}
		}

		return result;
	}

	std::vector<const Entity*> World::FindEntitiesWithName(std::string_view name) const
	{
		std::vector<const Entity*> result;
		for (const std::unique_ptr<Entity>& entity : entities)
		{
			if (!entity->IsDestroyed() && entity->GetName() == name)
			{
				result.push_back(entity.get());
			}
		}

		return result;
	}

	Entity* World::FindEntityWithTag(std::string_view tag) noexcept
	{
		for (const std::unique_ptr<Entity>& entity : entities)
		{
			if (!entity->IsDestroyed() && entity->GetTag() == tag)
			{
				return entity.get();
			}
		}

		return nullptr;
	}

	const Entity* World::FindEntityWithTag(std::string_view tag) const noexcept
	{
		for (const std::unique_ptr<Entity>& entity : entities)
		{
			if (!entity->IsDestroyed() && entity->GetTag() == tag)
			{
				return entity.get();
			}
		}

		return nullptr;
	}

	std::vector<Entity*> World::FindEntitiesWithTag(std::string_view tag)
	{
		std::vector<Entity*> result;
		for (const std::unique_ptr<Entity>& entity : entities)
		{
			if (!entity->IsDestroyed() && entity->GetTag() == tag)
			{
				result.push_back(entity.get());
			}
		}

		return result;
	}

	std::vector<const Entity*> World::FindEntitiesWithTag(std::string_view tag) const
	{
		std::vector<const Entity*> result;
		for (const std::unique_ptr<Entity>& entity : entities)
		{
			if (!entity->IsDestroyed() && entity->GetTag() == tag)
			{
				result.push_back(entity.get());
			}
		}

		return result;
	}
}
