#include "Precompiled.hpp"
#include "Scene.hpp"

#include "../Graphics/Camera.hpp"
#include "../Graphics/GPUCommandBuffer.hpp"
#include "../Graphics/GPUTarget.hpp"
#include "../Graphics/MeshRenderer.hpp"
#include "../Graphics/GPUConstants.hpp"

#include "../Resources/ModelAsset.hpp"

#include "../Core/Common.hpp"

#include "GameObject.hpp"
#include "Transform.hpp"

namespace TUK::Framework
{
	template <class TCallback>
	void Scene::RunPhase(TCallback callback)
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
	void Scene::ForEachGameObject(TCallback callback)
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

	void Scene::Enter()
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

	void Scene::EarlyUpdate()
	{
		if (isEntered)
		{
			RunPhase([this]()
			{
				DestroyObjects();
			});
		}
	}

	void Scene::FixedUpdate()
	{
		UpdateObjects(&Scene::OnFixedUpdate, &GameObject::FixedUpdate);
	}

	void Scene::Update()
	{
		UpdateObjects(&Scene::OnUpdate, &GameObject::Update);
	}

	void Scene::LateUpdate()
	{
		UpdateObjects(&Scene::OnLateUpdate, &GameObject::LateUpdate);
	}

	void Scene::PreRender(GPUCommandBuffer& commandBuffer)
	{
		if (isEntered)
		{
			RunPhase([this, &commandBuffer]()
			{
				RenderObjects(&GameObject::PreRender, commandBuffer);
			});
		}
	}

	void Scene::Render(GPUCommandBuffer& commandBuffer)
	{
		if (!isEntered)
		{
			return;
		}

		RunPhase([this, &commandBuffer]()
		{
			for (const Camera* activeCamera : activeCameras)
			{
				ASSERT_FMT(activeCamera, "활성화된 카메라가 nullptr입니다.");

				if (!isEntered)
				{
					break;
				}

				if (!activeCamera->IsEnabled() || activeCamera->IsDestroyed())
				{
					continue;
				}

				const GameObject* owner = activeCamera->GetOwner();
				ASSERT_FMT(owner, "활성화된 카메라의 소유자가 nullptr입니다.");

				if (!owner->IsEnabled() || owner->IsDestroyed())
				{
					continue;
				}

				if (commandBuffer.GetActiveTargetFormat() == DXGI_FORMAT_UNKNOWN)
				{
					LOG_ERROR("카메라를 적용하기 전에 렌더 타깃을 설정해야 합니다.");
					continue;
				}

				const auto& viewport = activeCamera->GetViewport();
				const auto& scissor = activeCamera->GetScissorRect();
				const auto& bounds = commandBuffer.GetActiveTargetBounds();
				if (viewport.Width <= 0.0f || viewport.Height <= 0.0f)
				{
					continue;
				}

				const f32 targetWidth = static_cast<f32>(bounds.right);
				const f32 targetHeight = static_cast<f32>(bounds.bottom);
				const D3D12_RECT clearRect
				{
					std::max(scissor.left, static_cast<LONG>(std::floor(std::clamp(viewport.TopLeftX, 0.0f, targetWidth)))),
					std::max(scissor.top, static_cast<LONG>(std::floor(std::clamp(viewport.TopLeftY, 0.0f, targetHeight)))),
					std::min(scissor.right, static_cast<LONG>(std::ceil(std::clamp(viewport.TopLeftX + viewport.Width, 0.0f, targetWidth)))),
					std::min(scissor.bottom, static_cast<LONG>(std::ceil(std::clamp(viewport.TopLeftY + viewport.Height, 0.0f, targetHeight))))
				};
				if (clearRect.left >= clearRect.right || clearRect.top >= clearRect.bottom)
				{
					continue;
				}

				const CameraConstants constants
				{
					.view = activeCamera->GetViewMatrix(),
					.projection = activeCamera->GetProjectionMatrix()
				};
				if (auto uploaded = commandBuffer.SetGraphicsConstants(constants); !uploaded)
				{
					LOG_ERROR("월드의 카메라 상수 설정에 실패했습니다: {}", uploaded.error());
					continue;
				}

				commandBuffer.SetViewport(viewport);
				commandBuffer.SetScissorRect(scissor);
				const auto& clearColor = activeCamera->GetClearColor();
				const f32 color[]{ clearColor.GetR(), clearColor.GetG(), clearColor.GetB(), clearColor.GetA() };
				commandBuffer.ClearRenderTarget(color, clearRect);
				commandBuffer.ClearDepthStencil(GPUTarget::DEPTH_CLEAR_VALUE, clearRect);

				RenderObjects(&GameObject::Render, commandBuffer);
			}
		});
	}

	void Scene::PostRender(GPUCommandBuffer& commandBuffer)
	{
		if (isEntered)
		{
			RunPhase([this, &commandBuffer]()
			{
				RenderObjects(&GameObject::PostRender, commandBuffer);
			});
		}
	}

	void Scene::Exit()
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

	void Scene::UpdateObjects(SceneCallback sceneCallback, GameObjectCallback gameObjectCallback)
	{
		ASSERT_FMT(sceneCallback, "sceneCallback이 nullptr입니다.");
		ASSERT_FMT(gameObjectCallback, "gameObjectCallback이 nullptr입니다.");

		if (!isEntered)
		{
			return;
		}

		RunPhase([this, sceneCallback, gameObjectCallback]()
		{
			DestroyObjects();
			if (!isEntered)
			{
				return;
			}
			(this->*sceneCallback)();
			ForEachGameObject([gameObjectCallback](GameObject& gameObject)
			{
				(gameObject.*gameObjectCallback)();
			});
		});
	}

	void Scene::RenderObjects(RenderCallback callback, GPUCommandBuffer& commandBuffer)
	{
		ASSERT_FMT(callback, "callback이 비어 있습니다.");

		ForEachGameObject([callback, &commandBuffer](GameObject& gameObject)
		{
			(gameObject.*callback)(commandBuffer);
		});
	}

	GameObject* Scene::Instantiate()
	{
		auto gameObject = std::make_unique<GameObject>(this);
		GameObject* result = gameObject.get();
		objects.push_back(std::move(gameObject));
		return result;
	}

	GameObject* Scene::Instantiate(const Vector3D<f32>& position, const Quaternion<f32>& rotation)
	{
		return Instantiate(position, rotation, nullptr);
	}

	GameObject* Scene::Instantiate(const Vector3D<f32>& position)
	{
		GameObject* gameObject = Instantiate(static_cast<Transform*>(nullptr));
		gameObject->GetComponent<Transform>()->SetLocalPosition(position);
		return gameObject;
	}

	GameObject* Scene::Instantiate(const Quaternion<f32>& rotation)
	{
		GameObject* gameObject = Instantiate(static_cast<Transform*>(nullptr));
		gameObject->GetComponent<Transform>()->SetLocalRotation(rotation);
		return gameObject;
	}

	GameObject* Scene::Instantiate(Transform* parent)
	{
		GameObject* gameObject = Instantiate();
		gameObject->AddComponent<Transform>()->SetParent(parent);
		return gameObject;
	}

	GameObject* Scene::Instantiate(const Vector3D<f32>& position, const Quaternion<f32>& rotation, Transform* parent)
	{
		GameObject* gameObject = Instantiate(parent);
		Transform* transform = gameObject->GetComponent<Transform>();
		transform->SetLocalPosition(position);
		transform->SetLocalRotation(rotation);
		return gameObject;
	}

	GameObject* Scene::Instantiate(const ModelAsset* model)
	{
		return Instantiate(model, static_cast<Transform*>(nullptr));
	}

	GameObject* Scene::Instantiate(const ModelAsset* model, const Vector3D<f32>& position, const Quaternion<f32>& rotation)
	{
		return Instantiate(model, position, rotation, nullptr);
	}

	GameObject* Scene::Instantiate(const ModelAsset* model, const Vector3D<f32>& position)
	{
		GameObject* gameObject = Instantiate(model);
		if (gameObject)
		{
			gameObject->GetComponent<Transform>()->SetLocalPosition(position);
		}
		return gameObject;
	}

	GameObject* Scene::Instantiate(const ModelAsset* model, const Quaternion<f32>& rotation)
	{
		GameObject* gameObject = Instantiate(model);
		if (gameObject)
		{
			gameObject->GetComponent<Transform>()->SetLocalRotation(rotation);
		}
		return gameObject;
	}

	GameObject* Scene::Instantiate(const ModelAsset* modelAsset, Transform* parent)
	{
		ASSERT_FMT(modelAsset, "인스턴스화할 모델이 nullptr입니다.");

		const ModelAsset& model = *modelAsset;

		const std::span<const Submodel> submodels = model.GetSubmodels();
		if (submodels.empty())
		{
			LOG_ERROR("모델에 서브모델이 없습니다: {}", model.GetName());
			return nullptr;
		}

		for (const Submodel& submodel : submodels)
		{
			if ((submodel.mesh != nullptr) != (submodel.material != nullptr))
			{
				LOG_ERROR("모델 노드의 메시와 머티리얼은 함께 지정해야 합니다: {}", submodel.name);
				return nullptr;
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

			if (node.mesh && node.material)
			{
				MeshRenderer* renderer = gameObject->AddComponent<MeshRenderer>();
				renderer->SetMesh(node.mesh);
				renderer->SetMaterial(node.material);
			}

			nodeObjects[index] = gameObject;
			nodeTransforms[index] = transform;
		}
		return nodeObjects.front();
	}

	GameObject* Scene::Instantiate(
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

	void Scene::Destroy(GameObject* gameObject) noexcept
	{
		ASSERT_FMT(gameObject, "삭제할 엔티티가 nullptr입니다.");

		if (gameObject)
		{
			gameObject->Destroy();
		}
	}

	void Scene::DestroyObjects()
	{
		if (!isEntered)
		{
			return;
		}

		const auto destroyedCount = std::ranges::count_if(objects, [](const auto& gameObject) {
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

	void Scene::ClearObjects()
	{
		if (objects.empty())
		{
			activeCameras.clear();
			return;
		}

		std::vector<std::unique_ptr<GameObject>> removing = std::move(objects);
		objects.clear();

		for (std::unique_ptr<GameObject>& gameObject : removing | std::views::reverse)
		{
			gameObject->Destroy();
			gameObject.reset();
		}

		activeCameras.clear();
	}

	void Scene::AddCamera(Camera* camera)
	{
		ASSERT_FMT(camera, "등록할 카메라가 nullptr입니다.");

		if (camera && std::ranges::find(activeCameras, camera) == activeCameras.end())
		{
			activeCameras.push_back(camera);
		}
	}

	void Scene::RemoveCamera(const Camera* camera)
	{
		ASSERT_FMT(camera, "등록 해제할 카메라가 nullptr입니다.");

		std::erase(activeCameras, camera);
	}

	std::vector<std::unique_ptr<GameObject>> Scene::ExtractPersistentObjects()
	{
		std::vector<std::unique_ptr<GameObject>> extracted;
		for (auto object = objects.begin(); object != objects.end();)
		{
			GameObject* gameObject = object->get();
			if (gameObject->ShouldDontDestoryOnLoad() && !gameObject->IsDestroyed())
			{
				if (Camera* camera = gameObject->GetComponent<Camera>())
				{
					RemoveCamera(camera);
				}

				// 씬이 전환되어도 파괴되지 않는 객체는 따로 수집해둔다
				extracted.push_back(std::move(*object));
				object = objects.erase(object);
			}
			else
			{
				++object;
			}
		}

		return extracted;
	}

	void Scene::AdoptObjects(std::vector<std::unique_ptr<GameObject>>&& adopted)
	{
		for (std::unique_ptr<GameObject>& gameObject : adopted)
		{
			if (!gameObject)
			{
				continue;
			}

			gameObject->scene = this;
			if (Camera* camera = gameObject->GetComponent<Camera>())
			{
				AddCamera(camera);
			}

			objects.push_back(std::move(gameObject));
		}
	}

	GameObject* Scene::FindGameObjectByName(std::string_view name) noexcept
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

	const GameObject* Scene::FindGameObjectByName(std::string_view name) const noexcept
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

	std::vector<GameObject*> Scene::FindObjectsByName(std::string_view name)
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

	std::vector<const GameObject*> Scene::FindObjectsByName(std::string_view name) const
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

	GameObject* Scene::FindGameObjectByTag(std::string_view tag) noexcept
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

	const GameObject* Scene::FindGameObjectByTag(std::string_view tag) const noexcept
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

	std::vector<GameObject*> Scene::FindObjectsByTag(std::string_view tag)
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

	std::vector<const GameObject*> Scene::FindObjectsByTag(std::string_view tag) const
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
