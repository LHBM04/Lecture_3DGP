#include "Precompiled.h"
#include "Scene.h"

#include "Camera.h"
#include "GameObject.h"

void Scene::AddCamera(Camera* const camera_)
{
	if (nullptr == camera_ || std::find(cameras.begin(), cameras.end(), camera_) != cameras.end())
	{
		return;
	}

	cameras.push_back(camera_);
	if (nullptr == mainCamera)
	{
		mainCamera = camera_;
	}
}

void Scene::RemoveCamera(Camera* const camera_)
{
	cameras.erase(std::remove(cameras.begin(), cameras.end(), camera_), cameras.end());
	if (mainCamera == camera_)
	{
		mainCamera = cameras.empty() ? nullptr : cameras.front();
	}
}

void Scene::SetMainCamera(Camera* camera_)
{
	if (nullptr != camera_ && std::find(cameras.begin(), cameras.end(), camera_) == cameras.end())
	{
		AddCamera(camera_);
	}

	mainCamera = camera_;
}

Camera* Scene::GetMainCamera() noexcept
{
	return mainCamera;
}

const Camera* Scene::GetMainCamera() const noexcept
{
	return mainCamera;
}

const std::vector<Camera*>& Scene::GetCameras() const noexcept
{
	return cameras;
}

GameObject* Scene::AddGameObject()
{
	std::unique_ptr<GameObject> newObject = std::make_unique<GameObject>();
	newObject->SetCurrentScene(this);
	newObject->SetName("New Object");
	newObject->SetTag("Untagged");

	objects.push_back(std::move(newObject));
	return objects.back().get();
}

void Scene::RemoveGameObject(GameObject* const gameObject_)
{
	objects.erase(std::remove_if(objects.begin(), objects.end(),
		[gameObject_](const std::unique_ptr<GameObject>& object)
		{
			return object.get() == gameObject_;
		}), objects.end());
}

void Scene::Load()
{
	if (isLoaded)
	{
		return;
	}

	isLoaded = true;
	OnLoad();
}

void Scene::Update()
{
	if (!isLoaded)
	{
		return;
	}

	OnUpdate();

	for (const std::unique_ptr<GameObject>& object : objects)
	{
		object->Update();
	}

	objects.erase(std::remove_if(objects.begin(), objects.end(),
		[](const std::unique_ptr<GameObject>& object)
		{
			return object->IsDestroyed();
		}), objects.end());
}

void Scene::Render(Renderer& renderer_)
{
	if (!isLoaded)
	{
		return;
	}

	OnRender(renderer_);

	for (const std::unique_ptr<GameObject>& object : objects)
	{
		object->Render(renderer_);
	}
}

void Scene::Unload()
{
	if (!isLoaded)
	{
		return;
	}

	OnUnload();

	cameras.clear();
	mainCamera = nullptr;
	objects.clear();
	isLoaded = false;
}
