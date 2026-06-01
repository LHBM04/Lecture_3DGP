#include "Precompiled.h"
#include "Scene.h"

#include <unordered_map>
#include "Camera.h"
#include "Collider.h"
#include "Light.h"
#include "Logger.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
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
	PhysicsSystem::GetInstance().RegisterStaticObjectsToGrid();
	isLoaded = true;
}

void Scene::Unload()
{
	if (!isLoaded)
	{
		return;
	}

	PhysicsSystem::GetInstance().Clear();
	OnUnload();

	gameObjects.clear();
	cameras.clear();
	lights.clear();
	destroyQueue.clear();

	isLoaded = false;
}

void Scene::Update(float deltaTime_)
{
	if (!isLoaded)
	{
		return;
	}

	for (const std::unique_ptr<GameObject>& go : gameObjects)
	{
		if (go->IsDestroyed())
		{
			continue;
		}

		go->Update(deltaTime_);
	}

	ProcessDestroyQueue();
}

void Scene::FixedUpdate(float fixedDeltaTime_)
{
	if (!isLoaded)
	{
		return;
	}

	PhysicsSystem::GetInstance().Update(fixedDeltaTime_);

	for (const std::unique_ptr<GameObject>& go : gameObjects)
	{
		if (go->IsDestroyed())
		{
			continue;
		}
		go->FixedUpdate(fixedDeltaTime_);
	}
}

void Scene::Render()
{
	if (!isLoaded)
	{
		return;
	}

	for (Camera* const& camera : cameras)
	{
		if (camera == nullptr)
		{
			continue;
		}

		GameObject* cameraOwner{ camera->GetOwner() };
		if (cameraOwner == nullptr || cameraOwner->IsDestroyed() || !cameraOwner->IsActive())
		{
			continue;
		}

		RenderSystem::GetInstance().SetCamera(camera);
		RenderSystem::GetInstance().SetLights(lights);

		RenderSystem::GetInstance().SetPipelineState(RenderSystem::GetInstance().GetDefaultPipelineState());

		struct BatchKey final
		{
			Mesh* mesh;
			Material* material;

			bool operator==(const BatchKey& other_) const noexcept
			{
				return mesh == other_.mesh && material == other_.material;
			}
		};

		struct BatchKeyHash final
		{
			std::size_t operator()(const BatchKey& key_) const noexcept
			{
				const std::size_t meshHash{ std::hash<Mesh*>{}(key_.mesh) };
				const std::size_t materialHash{ std::hash<Material*>{}(key_.material) };
				return meshHash ^ (materialHash << 1);
			}
		};

		std::unordered_map<BatchKey, std::vector<Matrix4x4>, BatchKeyHash> batches;
		std::vector<GameObject*> fallbackRenderObjects;

		for (const std::unique_ptr<GameObject>& gameObject : gameObjects)
		{
			if (gameObject == nullptr || !gameObject->IsActive() || gameObject->IsDestroyed() || gameObject->GetTag() == L"UI")
			{
				continue;
			}

			bool inFrustum{ true };
			if (Collider* col{ gameObject->GetComponent<Collider>() }; col != nullptr)
			{
				inFrustum = col->IsIntersects(camera->GetFrustum());
			}

			if (!inFrustum)
			{
				continue;
			}

			if (MeshRenderer* meshRenderer{ gameObject->GetComponent<MeshRenderer>() }; meshRenderer != nullptr)
			{
				Mesh* const mesh{ meshRenderer->GetMesh() };
				Material* const material{ meshRenderer->GetMaterial() };
				Transform* const transform{ gameObject->GetComponent<Transform>() };

				if (mesh != nullptr && material != nullptr && transform != nullptr)
				{
					batches[{ mesh, material }].emplace_back(transform->GetWorldMatrix());
					continue;
				}
			}

			fallbackRenderObjects.emplace_back(gameObject.get());
		}

		for (auto& [key, transforms] : batches)
		{
			RenderSystem::GetInstance().DrawMeshInstanced(key.mesh, key.material, transforms);
		}

		for (GameObject* gameObject : fallbackRenderObjects)
		{
			if (gameObject != nullptr)
			{
				gameObject->Render();
			}
		}

		RenderSystem::GetInstance().ExecuteLightingPass();

		RenderSystem::GetInstance().SetRenderTargetToBackBuffer();
		RenderSystem::GetInstance().SetPipelineState(RenderSystem::GetInstance().GetUIPipelineState());
		for (const std::unique_ptr<GameObject>& gameObject : gameObjects)
		{
			if (gameObject == nullptr || !gameObject->IsActive() || gameObject->IsDestroyed() || gameObject->GetTag() != L"UI")
			{
				continue;
			}

			gameObject->Render();
		}
	}
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
	gameObjects.push_back(std::move(newObject));
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

		current->Destroy();
		if (std::find(destroyQueue.begin(), destroyQueue.end(), current) == destroyQueue.end())
		{
			destroyQueue.emplace_back(current);
		}
	}
}

GameObject* Scene::Pick(const Vector3D& rayOrigin_, const Vector3D& rayDir_, float* distance_)
{
	return PhysicsSystem::GetInstance().Raycast(rayOrigin_, rayDir_, distance_);
}

GameObject* Scene::FindObjectWithName(std::wstring_view name_)
{
	std::vector<std::unique_ptr<GameObject>>::iterator found{ std::ranges::find_if(gameObjects, 
		[name_](const std::unique_ptr<GameObject>& go) { return go->GetName() == name_; }) };
	
	if (found != gameObjects.end())
	{
		return found->get();
	}
	return nullptr;
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

void Scene::ProcessDestroyQueue()
{
	if (destroyQueue.empty())
	{
		return;
	}

	for (GameObject* gameObject : destroyQueue)
	{
		if (gameObject == nullptr)
		{
			continue;
		}

		cameras.erase(std::remove_if(cameras.begin(), cameras.end(),
			[gameObject](Camera* camera) { return camera == nullptr || camera->GetOwner() == gameObject; }),
			cameras.end());

		lights.erase(std::remove_if(lights.begin(), lights.end(),
			[gameObject](Light* light) { return light == nullptr || light->GetOwner() == gameObject; }),
			lights.end());
	}

	std::erase_if(gameObjects, [](const std::unique_ptr<GameObject>& go)
	{
		return go == nullptr || go->IsDestroyed();
	});

	destroyQueue.clear();
}

