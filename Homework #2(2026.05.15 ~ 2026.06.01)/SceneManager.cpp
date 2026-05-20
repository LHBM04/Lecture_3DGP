#include "Precompiled.h"
#include "SceneManager.h"

void SceneManager::LoadScene(std::unique_ptr<Scene> scene_)
{
	UnloadScene();
	currentScene = std::move(scene_);

	if (nullptr != currentScene)
	{
		currentScene->Load();
	}
}

void SceneManager::Update()
{
	if (nullptr != currentScene)
	{
		currentScene->Update();
	}
}

void SceneManager::Render(Renderer& renderer_)
{
	if (nullptr != currentScene)
	{
		currentScene->Render(renderer_);
	}
}

void SceneManager::UnloadScene()
{
	if (nullptr != currentScene)
	{
		currentScene->Unload();
		currentScene.reset();
	}
}

Scene* SceneManager::GetCurrentScene() noexcept
{
	return currentScene.get();
}
