#include "Precompiled.h"

#include "Scene.h"

#include "Camera.h"
#include "Collider.h"
#include "GameObject.h"
#include "PhysicsSystem.h"
#include "RenderSystem.h"
#include "Transform.h"

Scene::Scene() = default;
Scene::~Scene() = default;

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
	isLoaded = false;
}

void Scene::Update(float deltaTime_)
{
	assert(isLoaded);

	for (const std::unique_ptr<GameObject>& go : gameObjects)
	{
		go->Update(deltaTime_);
	}
}

void Scene::FixedUpdate(float fixedDeltaTime_)
{
	assert(isLoaded);

	PhysicsSystem::GetInstance().Update(fixedDeltaTime_);

	for (const std::unique_ptr<GameObject>& go : gameObjects)
	{
		go->FixedUpdate(fixedDeltaTime_);
	}
}

void Scene::LateUpdate(float deltaTime_)
{
	assert(isLoaded);

	for (const std::unique_ptr<GameObject>& go : gameObjects)
	{
		go->LateUpdate(deltaTime_);
	}
}

void Scene::Render()
{
	assert(isLoaded);

	for (Camera* const& camera : cameras)
	{
		if (!camera->GetOwner()->IsActive())
		{
			continue;
		}

		RenderSystem::GetInstance().SetCamera(camera);
		RenderSystem::GetInstance().SetLights(lights);

		for (const std::unique_ptr<GameObject>& gameObject : gameObjects)
		{
			if (!gameObject->IsActive())
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

			gameObject->Render();
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

std::vector<GameObject*> Scene::FindObjectsWithName(std::wstring_view name_)
{
	std::vector<GameObject*> result;
	for (const std::unique_ptr<GameObject>& go : gameObjects)
	{
		if (go->GetName() == name_)
		{
			result.push_back(go.get());
		}
	}
	return result;
}

std::vector<GameObject*> Scene::FindObjectsWithTag(std::wstring_view tag_)
{
	std::vector<GameObject*> result;
	for (const std::unique_ptr<GameObject>& go : gameObjects)
	{
		if (go->GetTag() == tag_)
		{
			result.push_back(go.get());
		}
	}
	return result;
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

	return newObjectPtr;
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
