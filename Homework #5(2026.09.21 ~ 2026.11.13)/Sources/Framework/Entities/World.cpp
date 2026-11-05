#include "Precompiled.hpp"
#include "World.hpp"

#include <algorithm>
#include <filesystem>
#include <string>
#include <utility>
#include <ranges>

#include "../Graphics/Camera.hpp"
#include "../Graphics/MeshRenderer.hpp"

#include "../Resources/AssetSubsystem.hpp"
#include "../Resources/MaterialAsset.hpp"
#include "../Resources/MeshAsset.hpp"
#include "../Resources/ModelAsset.hpp"
#include "../Resources/ShaderAsset.hpp"

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

	std::expected<Entity*, std::string> World::Instantiate(
		const ModelAsset& model,
		const std::filesystem::path& modelPath,
		AssetSubsystem& assets,
		const GPUDevice& device,
		const Vector3D<f32>& position,
		Transform* parent)
	{
		constexpr DXGI_FORMAT TARGET_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
		struct PreparedRenderer
		{
			const MeshAsset* mesh;
			const MaterialAsset* material;
			GPUGraphicsPipelineState pipeline;
		};

		const auto resourcePath = [&modelPath](std::string_view directory, std::string_view name)
		{
			std::filesystem::path path(std::u8string(name.begin(), name.end()));
			if (!path.has_parent_path())
			{
				path = modelPath.parent_path().parent_path() / directory / path;
			}
			if (path.extension() != ".bin")
			{
				path += ".bin";
			}
			return path.lexically_normal();
		};
		const auto shaderPath = [](const std::filesystem::path& materialPath, std::string_view name)
		{
			std::filesystem::path path(std::u8string(name.begin(), name.end()));
			if (path.is_absolute())
			{
				return path.lexically_normal();
			}
			if (!path.empty() && *path.begin() == "Resources")
			{
				return (materialPath.parent_path().parent_path() / path.lexically_relative("Resources")).lexically_normal();
			}
			return (materialPath.parent_path() / path).lexically_normal();
		};

		const std::span<const ModelNode> nodes = model.GetNodes();
		std::vector<std::vector<PreparedRenderer>> prepared(nodes.size());
		for (usize index = 0; index < nodes.size(); ++index)
		{
			for (const ModelRendererReference& reference : nodes[index].renderers)
			{
				const auto meshPath = resourcePath("Meshes", reference.meshName);
				const auto materialPath = resourcePath("Materials", reference.materialName);
				const MeshAsset* mesh = assets.LoadAsset<MeshAsset>(meshPath);
				if (!mesh)
				{
					return std::unexpected("Cannot load mesh: " + meshPath.generic_string());
				}
				const MaterialAsset* material = assets.LoadAsset<MaterialAsset>(materialPath);
				if (!material)
				{
					return std::unexpected("Cannot load material: " + materialPath.generic_string());
				}
				const auto vertexPath = shaderPath(materialPath, material->GetVertexShaderPath());
				const auto pixelPath = shaderPath(materialPath, material->GetPixelShaderPath());
				const ShaderAsset* vertex = assets.LoadAsset<ShaderAsset>(vertexPath);
				if (!vertex)
				{
					return std::unexpected("Cannot load vertex shader: " + vertexPath.generic_string());
				}
				const ShaderAsset* pixel = assets.LoadAsset<ShaderAsset>(pixelPath);
				if (!pixel)
				{
					return std::unexpected("Cannot load pixel shader: " + pixelPath.generic_string());
				}
				auto pipeline = GPUGraphicsPipelineState::CreateForMaterial(device, *material, *vertex, *pixel, TARGET_FORMAT);
				if (!pipeline)
				{
					return std::unexpected("Cannot create pipeline for " + materialPath.generic_string() + ": " + pipeline.error());
				}
				prepared[index].push_back(PreparedRenderer{ mesh, material, std::move(*pipeline) });
			}
		}

		if (nodes.empty())
		{
			Entity& entity = Instantiate(model.GetName());
			Transform* transform = entity.AddComponent<Transform>();
			transform->SetParent(parent);
			transform->SetLocalPosition(position);
			return &entity;
		}

		std::vector<Entity*> nodeEntities(nodes.size(), nullptr);
		std::vector<Transform*> nodeTransforms(nodes.size(), nullptr);
		for (usize index = 0; index < nodes.size(); ++index)
		{
			const ModelNode& node = nodes[index];
			const std::string_view entityName = !node.name.empty() ? std::string_view(node.name) : model.GetName();
			Entity& entity = Instantiate(entityName);
			Transform* transform = entity.AddComponent<Transform>();
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
			for (PreparedRenderer& reference : prepared[index])
			{
				MeshRenderer* renderer = entity.AddComponent<MeshRenderer>();
				renderer->SetMesh(*reference.mesh);
				renderer->SetMaterial(*reference.material);
				renderer->SetPipelineState(std::move(reference.pipeline), TARGET_FORMAT);
			}
			nodeEntities[index] = &entity;
			nodeTransforms[index] = transform;
		}
		nodeTransforms.front()->SetLocalPosition(position);
		return nodeEntities.front();
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
