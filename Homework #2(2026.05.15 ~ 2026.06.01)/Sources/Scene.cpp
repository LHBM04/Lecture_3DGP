#include "Precompiled.h"
#include "Scene.h"

#include "Camera.h"
#include "GameObject.h"
#include "ResourceManager.h"

void Scene::AddCamera(Camera* camera_)
{
	if (nullptr == camera_ || cameras.end() != std::find(cameras.begin(), cameras.end(), camera_))
	{
		return;
	}

	cameras.push_back(camera_);
}

void Scene::RemoveCamera(Camera* camera_)
{
	std::erase(cameras, camera_);
}

GameObject& Scene::CreateGameObject(const std::string& name_)
{
	std::unique_ptr<GameObject> object{ std::make_unique<GameObject>() };
	object->currentScene = this;
	object->SetName(name_);

	GameObject& reference{ *object };
	gameObjects.emplace_back(std::move(object));
	return reference;
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

	for (const std::unique_ptr<GameObject>& object : gameObjects)
	{
		if (object->IsActive())
		{
			object->Update();
		}
	}

	std::erase_if(gameObjects, [](const auto& object)
		{
			return object->IsDestroyed();
		});
}

void Scene::Render()
{
	Renderer& renderer{ Renderer::GetInstance() };

	for (Camera* camera : cameras)
	{
		renderer.SetCamera(camera);

		for (const std::unique_ptr<GameObject>& gameObject : gameObjects)
		{
			if (gameObject->IsActive())
			{
				gameObject->Render();
			}
		}

		renderer.Flush();
	}
}

void Scene::Unload()
{
	if (!isLoaded)
	{
		return;
	}

	OnUnload();
	isLoaded = false;
}
