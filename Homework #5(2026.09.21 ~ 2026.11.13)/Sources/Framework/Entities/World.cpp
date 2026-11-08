#include "Precompiled.hpp"
#include "World.hpp"

#include "../Core/App.hpp"

#include "../Graphics/Camera.hpp"
#include "../Graphics/GPUCommandBuffer.hpp"
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
	template <class TCallback>
	void World::RunPhase(TCallback callback)
	{
		if (isRunningPhase)
		{
			return;
		}

		isRunningPhase = true;
		callback();

		if (!isEntered)
		{
			ClearEntities();
		}
		isRunningPhase = false;
	}

	template <class TCallback>
	void World::ForEachEntity(TCallback callback)
	{
		const usize count = entities.size();
		for (usize index = 0; index < count && isEntered; ++index)
		{
			Entity& entity = *entities[index];
			if (!entity.IsDestroyed())
			{
				callback(entity);
			}
		}
	}

	void World::Enter()
	{
		if (isEntered)
		{
			return;
		}

		RunPhase([this]()
		{
			isEntered = true;
			OnEnter();
		});
	}

	void World::EarlyUpdate()
	{
		if (isEntered)
		{
			RunPhase([this]()
			{
				DestroyEntities();
			});
		}
	}

	void World::FixedUpdate()
	{
		UpdateEntities(&World::OnFixedUpdate, &Entity::FixedUpdate);
	}

	void World::Update()
	{
		UpdateEntities(&World::OnUpdate, &Entity::Update);
	}

	void World::LateUpdate()
	{
		UpdateEntities(&World::OnLateUpdate, &Entity::LateUpdate);
	}

	void World::PreRender(GPUCommandBuffer& commandBuffer)
	{
		if (isEntered)
		{
			RunPhase([this, &commandBuffer]()
			{
				RenderEntities(&Entity::PreRender, commandBuffer);
			});
		}
	}

	void World::Render(GPUCommandBuffer& commandBuffer)
	{
		if (!isEntered)
		{
			return;
		}

		RunPhase([this, &commandBuffer]()
		{
			// 렌더 콜백은 카메라를 활성화하거나 비활성화할 수 있습니다.
			const auto frameCameras = cameras;
			for (Camera* camera : frameCameras)
			{
				if (!isEntered)
				{
					break;
				}
				if (!camera->IsEnabled() || camera->IsDestroyed())
				{
					continue;
				}

				const Entity* owner = camera->GetOwner();
				if (!owner || !owner->IsEnabled() || owner->IsDestroyed())
				{
					continue;
				}

				if (auto result = commandBuffer.SetCamera(camera); !result)
				{
					LOG_ERROR("월드의 카메라 설정에 실패했습니다: {}", result.error());
					continue;
				}

				RenderEntities(&Entity::Render, commandBuffer);
			}
		});
	}

	void World::PostRender(GPUCommandBuffer& commandBuffer)
	{
		if (isEntered)
		{
			RunPhase([this, &commandBuffer]()
			{
				RenderEntities(&Entity::PostRender, commandBuffer);
			});
		}
	}

	void World::Exit()
	{
		const auto exit = [this]()
		{
			if (isEntered)
			{
				isEntered = false;
				OnExit();
			}
		};

		if (isRunningPhase)
		{
			exit();
		}
		else
		{
			RunPhase(exit);
		}
	}

	void World::UpdateEntities(WorldCallback worldCallback, EntityCallback entityCallback)
	{
		ASSERT_FMT(worldCallback, "검증 실패: {}", "worldCallback");
		ASSERT_FMT(entityCallback, "검증 실패: {}", "entityCallback");

		if (!isEntered)
		{
			return;
		}

		RunPhase([this, worldCallback, entityCallback]()
		{
			DestroyEntities();
			if (!isEntered)
			{
				return;
			}
			(this->*worldCallback)();
			ForEachEntity([entityCallback](Entity& entity)
			{
				(entity.*entityCallback)();
			});
		});
	}

	void World::RenderEntities(RenderCallback callback, GPUCommandBuffer& commandBuffer)
	{
		ASSERT_FMT(callback, "검증 실패: {}", "callback");

		ForEachEntity([callback, &commandBuffer](Entity& entity)
		{
			(entity.*callback)(commandBuffer);
		});
	}

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
		ASSERT_FMT(modelAsset, "인스턴스화할 모델이 nullptr입니다.");

		if (!modelAsset)
		{
			return nullptr;
		}

		const ModelAsset& model = *modelAsset;

		const std::span<const Submodel> submodels = model.GetSubmodels();
		if (submodels.empty())
		{
			LOG_ERROR("모델에 서브모델이 없습니다: {}", model.GetName());
			return nullptr;
		}

		const AssetSubsystem* assets = App::GetInstance().GetSubsystem<AssetSubsystem>();
		if (!assets)
		{
			LOG_ERROR("모델에 사용할 에셋 서브시스템이 없습니다: {}", model.GetName());
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
					LOG_ERROR("메시가 로드되지 않았습니다: {} (모델: {})", meshPath.generic_string(), model.GetName());
					return nullptr;
				}

				const MaterialAsset* material = assets->FindLoadedAsset<MaterialAsset>(materialPath);
				if (!material || !material->GetGraphicsPipelineState())
				{
					LOG_ERROR("머티리얼이 준비되지 않았습니다: {} (모델: {})", materialPath.generic_string(), model.GetName());
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

			Entity* entity = Instantiate();
			entity->SetName(model.GetName());

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
		ASSERT_FMT(entity, "삭제할 엔티티가 nullptr입니다.");

		if (entity)
		{
			entity->Destroy();
		}
	}

	void World::DestroyEntities()
	{
		if (!isEntered)
		{
			return;
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
		ASSERT_FMT(camera, "등록할 카메라가 nullptr입니다.");

		if (camera && std::ranges::find(cameras, camera) == cameras.end())
		{
			cameras.push_back(camera);
		}
	}

	void World::RemoveCamera(const Camera* camera)
	{
		ASSERT_FMT(camera, "등록 해제할 카메라가 nullptr입니다.");

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
			if (!entity->IsDestroyed() && entity->CompareTag(tag))
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
			if (!entity->IsDestroyed() && entity->CompareTag(tag))
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
			if (!entity->IsDestroyed() && entity->CompareTag(tag))
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
			if (!entity->IsDestroyed() && entity->CompareTag(tag))
			{
				result.push_back(entity.get());
			}
		}

		return result;
	}
}
