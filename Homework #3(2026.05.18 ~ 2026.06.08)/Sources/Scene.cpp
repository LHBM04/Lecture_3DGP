#include "Precompiled.h"
#include "Scene.h"

#include <cstdint>
#include <unordered_map>
#include <format>
#include "Camera.h"
#include "Light.h"
#include "Logger.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "Model.h"
#include "PhysicsSystem.h"
#include "Quaternion.h"
#include "RenderSystem.h"
#include "Transform.h"

void Scene::Load()
{
	if (isLoaded)
	{
		return;
	}

	OnLoad();
	isLoaded = true;
}

void Scene::Unload()
{
	if (!isLoaded)
	{
		return;
	}

	OnUnload();

	gameObjects.clear();
	addObjects.clear();
	cameras.clear();
	lights.clear();
	destroyObjects.clear();

	isLoaded = false;
}

void Scene::Update()
{
	if (!isLoaded)
	{
		return;
	}

	const std::size_t gameObjectCount{ gameObjects.size() };
	for (std::size_t index{ 0 }; index < gameObjectCount; ++index)
	{
		if (index >= gameObjects.size())
		{
			break;
		}

		GameObject* const gameObject{ gameObjects[index].get() };
		if (gameObject->IsDestroyed())
		{
			continue;
		}

		gameObject->Update();
	}

	OnUpdate();
}

void Scene::LateUpdate()
{
	if (!isLoaded)
	{
		return;
	}

	const std::size_t gameObjectCount{ gameObjects.size() };
	for (std::size_t index{ 0 }; index < gameObjectCount; ++index)
	{
		if (index >= gameObjects.size())
		{
			break;
		}

		GameObject* const gameObject{ gameObjects[index].get() };
		if (gameObject->IsDestroyed())
		{
			continue;
		}

		gameObject->LateUpdate();
	}
}

void Scene::FixedUpdate()
{
	if (!isLoaded)
	{
		return;
	}

	FlushPendingObjects();
	FlushDestroyObjects();

	const std::size_t gameObjectCount{ gameObjects.size() };
	for (std::size_t index{ 0 }; index < gameObjectCount; ++index)
	{
		if (index >= gameObjects.size())
		{
			break;
		}

		GameObject* const gameObject{ gameObjects[index].get() };
		if (gameObject->IsDestroyed())
		{
			continue;
		}

		gameObject->FixedUpdate();
	}

	OnFixedUpdate();
}

void Scene::Render()
{
	if (!isLoaded)
	{
		return;
	}

	Camera* const camera{ cameras.empty() ? nullptr : cameras.front() };
	if (camera != nullptr)
	{
		camera->UpdateFrustum();

		CameraConstants cameraData{};
		cameraData.viewMatrix = camera->GetViewMatrix();
		cameraData.projectionMatrix = camera->GetProjectionMatrix();
		cameraData.viewProjectionMatrix = camera->GetViewProjectionMatrix();
		RenderSystem::GetInstance().SetCameraConstants(cameraData);
	}

	LightConstants lightData{};
	if (!lights.empty() && lights.front() != nullptr)
	{
		lightData.lightDirection = lights.front()->GetLightDirection();
		lightData.lightColor = lights.front()->GetColor();
	}
	RenderSystem::GetInstance().SetLightConstants(lightData);

	const std::size_t gameObjectCount{ gameObjects.size() };
	for (std::size_t index{ 0 }; index < gameObjectCount; ++index)
	{
		if (index >= gameObjects.size())
		{
			break;
		}

		GameObject* const gameObject{ gameObjects[index].get() };
		if (gameObject == nullptr || gameObject->IsDestroyed())
		{
			continue;
		}

		gameObject->Render();
	}
}

void Scene::OnUpdate()
{
}

void Scene::OnFixedUpdate()
{
}

GameObject* Scene::Instantiate()
{
	std::unique_ptr<GameObject> newObject{ std::make_unique<GameObject>() };
	newObject->scene = this;
	newObject->SetName(L"New GameObject");
	newObject->SetTag(L"Untagged");

	Transform* transform{ newObject->AddComponent<Transform>() };
	transform->SetLocalPosition(Vector3D::GetZero());
	transform->SetLocalRotation(Quaternion::GetIdentity());
	transform->SetLocalScale(Vector3D::GetOne());
	transform->SetParent(nullptr);

	GameObject* newObjectPtr{ newObject.get() };
	addObjects.push_back(std::move(newObject));
	Logger::Trace(
		L"[Scene] Instantiate: scene_ptr=0x{:X}, object_ptr=0x{:X}, name={}",
		static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(this)),
		static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(newObjectPtr)),
		newObjectPtr->GetName());

	return newObjectPtr;
}

GameObject* Scene::Instantiate(const Vector3D& position_, const Quaternion& rotation_)
{
	GameObject* newObject{ Instantiate() };
	if (newObject == nullptr)
	{
		return nullptr;
	}

	if (Transform* transform{ newObject->GetComponent<Transform>() }; transform != nullptr)
	{
		transform->SetWorldPosition(position_);
		transform->SetWorldRotation(rotation_);
	}
	return newObject;
}

GameObject* Scene::Instantiate(const Vector3D& position_)
{
	return Instantiate(position_, Quaternion::GetIdentity());
}

GameObject* Scene::Instantiate(const Quaternion& rotation_)
{
	return Instantiate(Vector3D::GetZero(), rotation_);
}

GameObject* Scene::Instantiate(Transform* parent_)
{
	GameObject* newObject{ Instantiate() };
	if (newObject == nullptr)
	{
		return nullptr;
	}

	if (Transform* transform{ newObject->GetComponent<Transform>() }; transform != nullptr)
	{
		transform->SetParent(parent_);
	}
	return newObject;
}

GameObject* Scene::Instantiate(const Vector3D& position_, const Quaternion& rotation_, Transform* parent_)
{
	GameObject* newObject{ Instantiate(position_, rotation_) };
	if (newObject == nullptr)
	{
		return nullptr;
	}

	if (Transform* transform{ newObject->GetComponent<Transform>() }; transform != nullptr)
	{
		transform->SetParent(parent_);
	}
	return newObject;
}

GameObject* Scene::Instantiate(const Model* model_)
{
	if (model_ == nullptr)
	{
		return nullptr;
	}

	const std::vector<ModelNodeData>& nodes{ model_->GetNodes() };
	if (nodes.empty())
	{
		Logger::Critical(L"[Scene] Instantiate failed: model has no nodes. model={}", model_->GetModelName());
		return nullptr;
	}

	std::vector<GameObject*> createdObjects(nodes.size(), nullptr);

	for (std::size_t i{ 0 }; i < nodes.size(); ++i)
	{
		const ModelNodeData& node{ nodes[i] };

		GameObject* const gameObject{ Instantiate() };
		if (gameObject == nullptr)
		{
			Logger::Critical(L"[Scene] Instantiate failed: Instantiate returned null. model={}, node={}", model_->GetModelName(), node.name);
			return nullptr;
		}

		gameObject->SetName(node.name);
		createdObjects[i] = gameObject;
	}

	for (std::size_t i{ 0 }; i < nodes.size(); ++i)
	{
		const ModelNodeData& node{ nodes[i] };
		GameObject* const gameObject{ createdObjects[i] };
		Transform* const transform{ gameObject->GetComponent<Transform>() };
		if (transform == nullptr)
		{
			Logger::Critical(L"[Scene] Instantiate failed: node has no Transform. model={}, node={}", model_->GetModelName(), node.name);
			return nullptr;
		}

		if (node.parentIndex >= 0)
		{
			const std::size_t parentIndex{ static_cast<std::size_t>(node.parentIndex) };
			if (parentIndex >= createdObjects.size() || createdObjects[parentIndex] == nullptr)
			{
				Logger::Critical(L"[Scene] Instantiate failed: invalid parent index. model={}, node={}, parent={}", model_->GetModelName(), node.name, node.parentIndex);
				return nullptr;
			}

			Transform* const parentTransform{ createdObjects[parentIndex]->GetComponent<Transform>() };
			transform->SetParent(parentTransform);
		}

		transform->SetLocalPosition(node.localPosition);
		transform->SetLocalRotation(node.localRotation);
		transform->SetLocalScale(node.localScale);

		for (std::size_t rendererIndex{ 0 }; rendererIndex < node.renderers.size(); ++rendererIndex)
		{
			const ModelRendererData& renderer{ node.renderers[rendererIndex] };

			GameObject* const rendererObject{ Instantiate(transform) };
			if (rendererObject == nullptr)
			{
				Logger::Critical(L"[Scene] Instantiate failed: renderer object creation failed. model={}, node={}", model_->GetModelName(), node.name);
				return nullptr;
			}

			rendererObject->SetName(std::format(L"{}_Renderer{}", node.name, rendererIndex));

			MeshRenderer* const meshRenderer{ rendererObject->AddComponent<MeshRenderer>() };
			meshRenderer->SetMesh(renderer.mesh);
			meshRenderer->SetMaterial(renderer.material);
		}
	}

	for (GameObject* object : createdObjects)
	{
		if (object == nullptr)
		{
			continue;
		}

		Transform* const transform{ object->GetComponent<Transform>() };
		if (transform != nullptr && transform->GetParent() == nullptr)
		{
			return object;
		}
	}

	return createdObjects.front();
}

GameObject* Scene::Instantiate(const Model* model_, const Vector3D& position_, const Quaternion& rotation_)
{
	GameObject* const root{ Instantiate(model_) };
	if (root == nullptr)
	{
		return nullptr;
	}

	if (Transform* const transform{ root->GetComponent<Transform>() }; transform != nullptr)
	{
		transform->SetWorldPosition(position_);
		transform->SetWorldRotation(rotation_);
	}

	return root;
}

GameObject* Scene::Instantiate(const Model* model_, const Vector3D& position_)
{
	return Instantiate(model_, position_, Quaternion::GetIdentity());
}

GameObject* Scene::Instantiate(const Model* model_, const Quaternion& rotation_)
{
	return Instantiate(model_, Vector3D::GetZero(), rotation_);
}

GameObject* Scene::Instantiate(const Model* model_, Transform* parent_)
{
	GameObject* const root{ Instantiate(model_) };
	if (root == nullptr)
	{
		return nullptr;
	}

	if (Transform* const transform{ root->GetComponent<Transform>() }; transform != nullptr)
	{
		transform->SetParent(parent_);
	}

	return root;
}

GameObject* Scene::Instantiate(const Model* model_, const Vector3D& position_, const Quaternion& rotation_, Transform* parent_)
{
	GameObject* const root{ Instantiate(model_, position_, rotation_) };
	if (root == nullptr)
	{
		return nullptr;
	}

	if (Transform* const transform{ root->GetComponent<Transform>() }; transform != nullptr)
	{
		transform->SetParent(parent_);
	}

	return root;
}

void Scene::Destroy(GameObject* gameObject_)
{
	if (gameObject_ == nullptr || gameObject_->IsDestroyed())
	{
		return;
	}

	std::vector<GameObject*> pending;
	pending.emplace_back(gameObject_);

	while (!pending.empty())
	{
		GameObject* current{ pending.back() };
		pending.pop_back();

		if (current == nullptr || current->IsDestroyed())
		{
			continue;
		}

		if (Transform* transform{ current->GetComponent<Transform>() }; transform != nullptr)
		{
			for (Transform* childTransform : transform->GetChildren())
			{
				if (childTransform == nullptr)
				{
					continue;
				}

				GameObject* childOwner{ childTransform->GetOwner() };
				if (childOwner != nullptr && !childOwner->IsDestroyed())
				{
					pending.emplace_back(childOwner);
				}
			}
		}

		current->isDestroyPending = true;
		current->SetActive(false);
		if (std::find(destroyObjects.begin(), destroyObjects.end(), current) == destroyObjects.end())
		{
			destroyObjects.emplace_back(current);
		}
	}
}

GameObject* Scene::FindObjectWithName(std::wstring_view name_)
{
	std::vector<std::unique_ptr<GameObject>>::iterator result{ std::ranges::find_if(gameObjects, 
		[name_](const std::unique_ptr<GameObject>& go) { return go->GetName() == name_; }) };
	
	if (result != gameObjects.end())
	{
		return result->get();
	}

	return nullptr;
}

std::vector<GameObject*> Scene::FindObjectsWithName(std::wstring_view name_)
{
	std::vector<GameObject*> results;
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects
		| std::views::filter([name_](const std::unique_ptr<GameObject>& go)
		{
			return go != nullptr && !go->IsDestroyed() && go->GetName() == name_;
		}))
	{
		results.emplace_back(gameObject.get());
	}

	return results;
}

std::vector<const GameObject*> Scene::FindObjectsWithName(std::wstring_view name_) const
{
	std::vector<const GameObject*> results;
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects
		| std::views::filter([name_](const std::unique_ptr<GameObject>& go)
		{
			return go != nullptr && !go->IsDestroyed() && go->GetName() == name_;
		}))
	{
		results.emplace_back(gameObject.get());
	}

	return results;
}

GameObject* Scene::FindObjectWithTag(std::wstring_view tag_)
{
	std::vector<std::unique_ptr<GameObject>>::iterator found{ std::ranges::find_if(gameObjects,
		[tag_](const std::unique_ptr<GameObject>& go) { return go->GetTag() == tag_; }) };

	if (found != gameObjects.end())
	{
		return found->get();
	}
	return nullptr;
}

std::vector<GameObject*> Scene::FindObjectsWithTag(std::wstring_view tag_)
{
	std::vector<GameObject*> results;
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects
		| std::views::filter([tag_](const std::unique_ptr<GameObject>& go)
		{
			return go != nullptr && !go->IsDestroyed() && go->GetTag() == tag_;
		}))
	{
		results.emplace_back(gameObject.get());
	}

	return results;
}

std::vector<const GameObject*> Scene::FindObjectsWithTag(std::wstring_view tag_) const
{
	std::vector<const GameObject*> results;
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects
		| std::views::filter([tag_](const std::unique_ptr<GameObject>& go)
		{
			return go != nullptr && !go->IsDestroyed() && go->GetTag() == tag_;
		}))
	{
		results.emplace_back(gameObject.get());
	}

	return results;
}

std::span<const std::unique_ptr<GameObject>> Scene::GetGameObjects() const noexcept
{
	return gameObjects;
}

std::span<Camera* const> Scene::GetCameras()
{
	return cameras;
}

std::span<const Camera* const> Scene::GetCameras() const
{
	return cameras;
}

std::span<Light* const> Scene::GetLights()
{
	return lights;
}

std::span<const Light* const> Scene::GetLights() const
{
	return lights;
}

void Scene::AddCamera(Camera* camera_)
{
	cameras.push_back(camera_);
}

void Scene::RemoveCamera(Camera* camera_)
{
	cameras.erase(std::remove(cameras.begin(), cameras.end(), camera_), cameras.end());
}

void Scene::AddLight(Light* light_)
{
	lights.push_back(light_);
}

void Scene::RemoveLight(Light* light_)
{
	lights.erase(std::remove(lights.begin(), lights.end(), light_), lights.end());
}

void Scene::FlushPendingObjects()
{
	if (addObjects.empty())
	{
		return;
	}

	for (std::unique_ptr<GameObject>& gameObject : addObjects)
	{
		if (gameObject == nullptr)
		{
			continue;
		}
	}

	for (std::unique_ptr<GameObject>& gameObject : addObjects)
	{
		gameObjects.push_back(std::move(gameObject));
	}

	addObjects.clear();
}

void Scene::FlushDestroyObjects()
{
	if (destroyObjects.empty())
	{
		return;
	}

	PhysicsSystem::GetInstance().ApplyPendingColliderChanges();

	for (GameObject* gameObject : destroyObjects)
	{
		if (gameObject == nullptr)
		{
			continue;
		}

		gameObject->Destroy();
	}

	std::erase_if(gameObjects, [](const std::unique_ptr<GameObject>& gameObject)
	{
		return gameObject == nullptr || gameObject->IsDestroyPending() || gameObject->IsDestroyed();
	});

	std::erase_if(addObjects, [](const std::unique_ptr<GameObject>& gameObject)
	{
		return gameObject == nullptr || gameObject->IsDestroyPending() || gameObject->IsDestroyed();
	});

	destroyObjects.clear();
}

