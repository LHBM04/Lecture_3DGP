#include "Precompiled.hpp"
#include "World.hpp"

#include "../Graphics/Camera.hpp"
#include "../Graphics/GPUCommandBuffer.hpp"
#include "../Graphics/MeshRenderer.hpp"

#include "../Resources/ModelAsset.hpp"

#include "../Core/Common.hpp"

#include "GameObject.hpp"
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
			ClearObjects();
		}
		isRunningPhase = false;
	}

	template <class TCallback>
	void World::ForEachGameObject(TCallback callback)
	{
		const usize count = objects.size();
		for (usize index = 0; index < count && isEntered; ++index)
		{
			GameObject& gameObject = *objects[index];
			if (!gameObject.IsDestroyed())
			{
				callback(gameObject);
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
				DestroyObjects();
			});
		}
	}

	void World::FixedUpdate()
	{
		UpdateObjects(&World::OnFixedUpdate, &GameObject::FixedUpdate);
	}

	void World::Update()
	{
		UpdateObjects(&World::OnUpdate, &GameObject::Update);
	}

	void World::LateUpdate()
	{
		UpdateObjects(&World::OnLateUpdate, &GameObject::LateUpdate);
	}

	void World::PreRender(GPUCommandBuffer& commandBuffer)
	{
		if (isEntered)
		{
			RunPhase([this, &commandBuffer]()
			{
				RenderObjects(&GameObject::PreRender, commandBuffer);
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

				const GameObject* owner = camera->GetOwner();
				if (!owner || !owner->IsEnabled() || owner->IsDestroyed())
				{
					continue;
				}

				if (auto result = camera->PrepareRender(commandBuffer); !result)
				{
					LOG_ERROR("월드의 카메라 설정에 실패했습니다: {}", result.error());
					continue;
				}

				const Camera::BackgroundPass background = camera->PrepareBackground(commandBuffer, skyboxMaterial);

				RenderObjects(&GameObject::Render, commandBuffer);

				if (isEntered)
				{
					camera->RenderBackground(commandBuffer, background);
				}
			}
		});
	}

	void World::PostRender(GPUCommandBuffer& commandBuffer)
	{
		if (isEntered)
		{
			RunPhase([this, &commandBuffer]()
			{
				RenderObjects(&GameObject::PostRender, commandBuffer);
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
				skyboxMaterial = nullptr;
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

	void World::UpdateObjects(WorldCallback worldCallback, GameObjectCallback gameObjectCallback)
	{
		ASSERT_FMT(worldCallback, "worldCallback이 nullptr입니다.");
		ASSERT_FMT(gameObjectCallback, "gameObjectCallback이 nullptr입니다.");

		if (!isEntered)
		{
			return;
		}

		RunPhase([this, worldCallback, gameObjectCallback]()
		{
			DestroyObjects();
			if (!isEntered)
			{
				return;
			}
			(this->*worldCallback)();
			ForEachGameObject([gameObjectCallback](GameObject& gameObject)
			{
				(gameObject.*gameObjectCallback)();
			});
		});
	}

	void World::RenderObjects(RenderCallback callback, GPUCommandBuffer& commandBuffer)
	{
		ASSERT_FMT(callback, "callback이 비어 있습니다.");

		ForEachGameObject([callback, &commandBuffer](GameObject& gameObject)
		{
			(gameObject.*callback)(commandBuffer);
		});
	}

	GameObject* World::Instantiate()
	{
		auto gameObject = std::make_unique<GameObject>(this);
		GameObject* result = gameObject.get();
		objects.push_back(std::move(gameObject));
		return result;
	}

	GameObject* World::Instantiate(const Vector3D<f32>& position, const Quaternion<f32>& rotation)
	{
		return Instantiate(position, rotation, nullptr);
	}

	GameObject* World::Instantiate(const Vector3D<f32>& position)
	{
		GameObject* gameObject = Instantiate(static_cast<Transform*>(nullptr));
		gameObject->GetComponent<Transform>()->SetLocalPosition(position);
		return gameObject;
	}

	GameObject* World::Instantiate(const Quaternion<f32>& rotation)
	{
		GameObject* gameObject = Instantiate(static_cast<Transform*>(nullptr));
		gameObject->GetComponent<Transform>()->SetLocalRotation(rotation);
		return gameObject;
	}

	GameObject* World::Instantiate(Transform* parent)
	{
		GameObject* gameObject = Instantiate();
		gameObject->AddComponent<Transform>()->SetParent(parent);
		return gameObject;
	}

	GameObject* World::Instantiate(const Vector3D<f32>& position, const Quaternion<f32>& rotation, Transform* parent)
	{
		GameObject* gameObject = Instantiate(parent);
		Transform* transform = gameObject->GetComponent<Transform>();
		transform->SetLocalPosition(position);
		transform->SetLocalRotation(rotation);
		return gameObject;
	}

	GameObject* World::Instantiate(const ModelAsset* model)
	{
		return Instantiate(model, static_cast<Transform*>(nullptr));
	}

	GameObject* World::Instantiate(const ModelAsset* model, const Vector3D<f32>& position, const Quaternion<f32>& rotation)
	{
		return Instantiate(model, position, rotation, nullptr);
	}

	GameObject* World::Instantiate(const ModelAsset* model, const Vector3D<f32>& position)
	{
		GameObject* gameObject = Instantiate(model);
		if (gameObject)
		{
			gameObject->GetComponent<Transform>()->SetLocalPosition(position);
		}
		return gameObject;
	}

	GameObject* World::Instantiate(const ModelAsset* model, const Quaternion<f32>& rotation)
	{
		GameObject* gameObject = Instantiate(model);
		if (gameObject)
		{
			gameObject->GetComponent<Transform>()->SetLocalRotation(rotation);
		}
		return gameObject;
	}

	GameObject* World::Instantiate(const ModelAsset* modelAsset, Transform* parent)
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

		for (const Submodel& submodel : submodels)
		{
			for (const Submodel::RenderPart& part : submodel.renderParts)
			{
				if (!part.mesh || !part.material)
				{
					LOG_ERROR("모델의 렌더 파트가 유효하지 않습니다: {}", model.GetName());
					return nullptr;
				}
			}
		}

		std::vector<GameObject*> nodeObjects(submodels.size(), nullptr);
		std::vector<Transform*> nodeTransforms(submodels.size(), nullptr);
		for (usize index = 0; index < submodels.size(); ++index)
		{
			const Submodel& node = submodels[index];

			GameObject* gameObject = Instantiate();
			gameObject->SetName(model.GetName());

			Transform* transform = gameObject->AddComponent<Transform>();
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

			for (const Submodel::RenderPart& part : node.renderParts)
			{
				GameObject* partGameObject = Instantiate(transform);
				MeshRenderer* renderer = partGameObject->AddComponent<MeshRenderer>();
				renderer->SetMesh(part.mesh);
				renderer->SetMaterial(part.material);
			}

			nodeObjects[index] = gameObject;
			nodeTransforms[index] = transform;
		}
		return nodeObjects.front();
	}

	GameObject* World::Instantiate(
		const ModelAsset* model,
		const Vector3D<f32>& position,
		const Quaternion<f32>& rotation,
		Transform* parent)
	{
		GameObject* gameObject = Instantiate(model, parent);
		if (gameObject)
		{
			Transform* transform = gameObject->GetComponent<Transform>();
			transform->SetLocalPosition(position);
			transform->SetLocalRotation(rotation);
		}
		return gameObject;
	}

	void World::Destroy(GameObject* gameObject) noexcept
	{
		ASSERT_FMT(gameObject, "삭제할 엔티티가 nullptr입니다.");

		if (gameObject)
		{
			gameObject->Destroy();
		}
	}

	void World::DestroyObjects()
	{
		if (!isEntered)
		{
			return;
		}

		const auto destroyedCount = std::ranges::count_if(objects,
			[](const auto& gameObject)
			{
				return gameObject->IsDestroyed();
			});
		if (destroyedCount == 0)
		{
			return;
		}

		std::vector<std::unique_ptr<GameObject>> destroyed;
		destroyed.reserve(static_cast<usize>(destroyedCount));

		usize remainingCount = 0;
		for (usize index = 0; index < objects.size(); ++index)
		{
			if (objects[index]->IsDestroyed())
			{
				destroyed.push_back(std::move(objects[index]));
			}
			else
			{
				if (remainingCount != index)
				{
					objects[remainingCount] = std::move(objects[index]);
				}
				++remainingCount;
			}
		}
		objects.resize(remainingCount);
	}

	void World::ClearObjects()
	{
		if (objects.empty())
		{
			cameras.clear();
			return;
		}

		std::vector<std::unique_ptr<GameObject>> removing = std::move(objects);
		objects.clear();

		for (std::unique_ptr<GameObject>& gameObject : removing | std::views::reverse)
		{
			gameObject->Destroy();
			gameObject.reset();
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

	GameObject* World::FindGameObjectByName(std::string_view name) noexcept
	{
		for (const std::unique_ptr<GameObject>& gameObject : objects)
		{
			if (!gameObject->IsDestroyed() && gameObject->GetName() == name)
			{
				return gameObject.get();
			}
		}

		return nullptr;
	}

	const GameObject* World::FindGameObjectByName(std::string_view name) const noexcept
	{
		for (const std::unique_ptr<GameObject>& gameObject : objects)
		{
			if (!gameObject->IsDestroyed() && gameObject->GetName() == name)
			{
				return gameObject.get();
			}
		}

		return nullptr;
	}

	std::vector<GameObject*> World::FindObjectsByName(std::string_view name)
	{
		std::vector<GameObject*> result;
		for (const std::unique_ptr<GameObject>& gameObject : objects)
		{
			if (!gameObject->IsDestroyed() && gameObject->GetName() == name)
			{
				result.push_back(gameObject.get());
			}
		}

		return result;
	}

	std::vector<const GameObject*> World::FindObjectsByName(std::string_view name) const
	{
		std::vector<const GameObject*> result;
		for (const std::unique_ptr<GameObject>& gameObject : objects)
		{
			if (!gameObject->IsDestroyed() && gameObject->GetName() == name)
			{
				result.push_back(gameObject.get());
			}
		}

		return result;
	}

	GameObject* World::FindGameObjectByTag(std::string_view tag) noexcept
	{
		for (const std::unique_ptr<GameObject>& gameObject : objects)
		{
			if (!gameObject->IsDestroyed() && gameObject->CompareTag(tag))
			{
				return gameObject.get();
			}
		}

		return nullptr;
	}

	const GameObject* World::FindGameObjectByTag(std::string_view tag) const noexcept
	{
		for (const std::unique_ptr<GameObject>& gameObject : objects)
		{
			if (!gameObject->IsDestroyed() && gameObject->CompareTag(tag))
			{
				return gameObject.get();
			}
		}

		return nullptr;
	}

	std::vector<GameObject*> World::FindObjectsByTag(std::string_view tag)
	{
		std::vector<GameObject*> result;
		for (const std::unique_ptr<GameObject>& gameObject : objects)
		{
			if (!gameObject->IsDestroyed() && gameObject->CompareTag(tag))
			{
				result.push_back(gameObject.get());
			}
		}

		return result;
	}

	std::vector<const GameObject*> World::FindObjectsByTag(std::string_view tag) const
	{
		std::vector<const GameObject*> result;
		for (const std::unique_ptr<GameObject>& gameObject : objects)
		{
			if (!gameObject->IsDestroyed() && gameObject->CompareTag(tag))
			{
				result.push_back(gameObject.get());
			}
		}

		return result;
	}
}
