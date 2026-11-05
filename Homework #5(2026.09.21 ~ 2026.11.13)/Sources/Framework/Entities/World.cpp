#include "Precompiled.hpp"
#include "World.hpp"

#include <algorithm>
#include <filesystem>
#include <ranges>
#include <string>
#include <utility>

#include "../Core/App.hpp"

#include "../Graphics/Camera.hpp"
#include "../Graphics/MeshRenderer.hpp"

#include "../Resources/AssetSubsystem.hpp"
#include "../Resources/MaterialAsset.hpp"
#include "../Resources/MeshAsset.hpp"
#include "../Resources/ModelAsset.hpp"
#include "../Utilities/Logger.hpp"

#include "Entity.hpp"
#include "Transform.hpp"

namespace TUK::Framework
{
	Entity* World::Instantiate()
	{
		auto entity = std::make_unique<Entity>(this);
		Entity* result = entity.get();
		entities.push_back(std::move(entity));
		return result;
	}

	Entity* World::Instantiate(const Vector3D<f32>& position, const Quaternion<f32>& rotation)
	{
		return Instantiate(position, rotation, nullptr);
	}

	Entity* World::Instantiate(const Vector3D<f32>& position)
	{
		Entity* entity = Instantiate(static_cast<Transform*>(nullptr));
		entity->GetComponent<Transform>()->SetLocalPosition(position);
		return entity;
	}

	Entity* World::Instantiate(const Quaternion<f32>& rotation)
	{
		Entity* entity = Instantiate(static_cast<Transform*>(nullptr));
		entity->GetComponent<Transform>()->SetLocalRotation(rotation);
		return entity;
	}

	Entity* World::Instantiate(Transform* parent)
	{
		Entity* entity = Instantiate();
		entity->AddComponent<Transform>()->SetParent(parent);
		return entity;
	}

	Entity* World::Instantiate(const Vector3D<f32>& position, const Quaternion<f32>& rotation, Transform* parent)
	{
		Entity* entity = Instantiate(parent);
		Transform* transform = entity->GetComponent<Transform>();
		transform->SetLocalPosition(position);
		transform->SetLocalRotation(rotation);
		return entity;
	}

	Entity* World::Instantiate(const ModelAsset* model)
	{
		return Instantiate(model, static_cast<Transform*>(nullptr));
	}

	Entity* World::Instantiate(const ModelAsset* model, const Vector3D<f32>& position, const Quaternion<f32>& rotation)
	{
		return Instantiate(model, position, rotation, nullptr);
	}

	Entity* World::Instantiate(const ModelAsset* model, const Vector3D<f32>& position)
	{
		Entity* entity = Instantiate(model);
		if (entity)
		{
			entity->GetComponent<Transform>()->SetLocalPosition(position);
		}
		return entity;
	}

	Entity* World::Instantiate(const ModelAsset* model, const Quaternion<f32>& rotation)
	{
		Entity* entity = Instantiate(model);
		if (entity)
		{
			entity->GetComponent<Transform>()->SetLocalRotation(rotation);
		}
		return entity;
	}

	Entity* World::Instantiate(const ModelAsset* modelAsset, Transform* parent)
	{
		if (!modelAsset)
		{
			return nullptr;
		}

		const ModelAsset& model = *modelAsset;
		const std::span<const Submodel> submodels = model.GetSubmodels();
		if (submodels.empty())
		{
			Logger::LogError("Model has no submodels: {}", model.GetName());
			return nullptr;
		}

		const AssetSubsystem* assets = App::GetInstance().GetSubsystem<AssetSubsystem>();
		if (!assets)
		{
			Logger::LogError("AssetSubsystem is unavailable for model: {}", model.GetName());
			return nullptr;
		}
		struct PreparedRenderer
		{
			const MeshAsset* mesh;
			const MaterialAsset* material;
		};

		const auto resourcePath = [&model](std::string_view directory, std::string_view name)
		{
			std::filesystem::path path(std::u8string(name.begin(), name.end()));
			if (!path.has_parent_path())
			{
				path = model.GetPath().parent_path().parent_path() / directory / path;
			}
			if (path.extension() != ".bin")
			{
				path += ".bin";
			}
			return path.lexically_normal();
		};
		std::vector<std::vector<PreparedRenderer>> prepared(submodels.size());
		for (usize index = 0; index < submodels.size(); ++index)
		{
			const Submodel& submodel = submodels[index];
			for (const Submodel::RenderPart& part : submodel.renderParts)
			{
				const auto meshPath = resourcePath("Meshes", part.meshName);
				const auto materialPath = resourcePath("Materials", part.materialName);
				const MeshAsset* mesh = assets->FindLoadedAsset<MeshAsset>(meshPath);
				if (!mesh)
				{
					Logger::LogError("Mesh {} was not loaded for model {}", meshPath.generic_string(), model.GetName());
					return nullptr;
				}
				const MaterialAsset* material = assets->FindLoadedAsset<MaterialAsset>(materialPath);
				if (!material || !material->GetGraphicsPipelineState())
				{
					Logger::LogError("Material {} was not prepared for model {}", materialPath.generic_string(), model.GetName());
					return nullptr;
				}
				prepared[index].push_back(PreparedRenderer{ mesh, material });
			}
		}

		std::vector<Entity*> nodeEntities(submodels.size(), nullptr);
		std::vector<Transform*> nodeTransforms(submodels.size(), nullptr);
		for (usize index = 0; index < submodels.size(); ++index)
		{
			const Submodel& node = submodels[index];
			const std::string_view entityName = !node.name.empty() ? std::string_view(node.name) : model.GetName();
			Entity* entity = Instantiate();
			entity->SetName(entityName);
			Transform* transform = entity->AddComponent<Transform>();
			if (node.parentIndex >= 0 && static_cast<usize>(node.parentIndex) < index)
			{
				transform->SetParent(nodeTransforms[static_cast<usize>(node.parentIndex)]);
			}
			else if (parent)
			{
				transform->SetParent(parent);
			}
			transform->SetLocalPosition(node.localPosition);
			transform->SetLocalRotation(node.localRotation);
			transform->SetLocalScale(node.localScale);
			for (const PreparedRenderer& reference : prepared[index])
			{
				Entity* part = Instantiate(transform);
				MeshRenderer* renderer = part->AddComponent<MeshRenderer>();
				renderer->SetMesh(reference.mesh);
				renderer->SetMaterial(reference.material);
			}
			nodeEntities[index] = entity;
			nodeTransforms[index] = transform;
		}
		return nodeEntities.front();
	}

	Entity* World::Instantiate(
		const ModelAsset* model,
		const Vector3D<f32>& position,
		const Quaternion<f32>& rotation,
		Transform* parent)
	{
		Entity* entity = Instantiate(model, parent);
		if (entity)
		{
			Transform* transform = entity->GetComponent<Transform>();
			transform->SetLocalPosition(position);
			transform->SetLocalRotation(rotation);
		}
		return entity;
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
				entity->DestroyComponents();
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

		std::vector<std::unique_ptr<Entity>> destroyed;
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

		std::vector<std::unique_ptr<Entity>> removing = std::move(entities);
		entities.clear();

		for (std::unique_ptr<Entity>& entity : removing | std::views::reverse)
		{
			entity->Destroy();
			entity.reset();
		}

		cameras.clear();
	}

	void World::AddCamera(Camera* camera)
	{
		if (camera && std::ranges::find(cameras, camera) == cameras.end())
		{
			cameras.push_back(camera);
		}
	}

	void World::RemoveCamera(const Camera* camera)
	{
		std::erase(cameras, camera);
	}

	Entity* World::FindEntityByName(std::string_view name) noexcept
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

	const Entity* World::FindEntityByName(std::string_view name) const noexcept
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

	std::vector<Entity*> World::FindEntitiesByName(std::string_view name)
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

	std::vector<const Entity*> World::FindEntitiesByName(std::string_view name) const
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

	Entity* World::FindEntityByTag(std::string_view tag) noexcept
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

	const Entity* World::FindEntityByTag(std::string_view tag) const noexcept
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

	std::vector<Entity*> World::FindEntitiesByTag(std::string_view tag)
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

	std::vector<const Entity*> World::FindEntitiesByTag(std::string_view tag) const
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
