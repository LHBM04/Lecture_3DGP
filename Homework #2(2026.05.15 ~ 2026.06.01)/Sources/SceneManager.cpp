#include "Precompiled.h"
#include "SceneManager.h"

#include "RenderSystem.h"

void SceneManager::Update(float deltaTime_)
{
	if (nextScene)
	{
		if (currentScene)
		{
			currentScene->Unload();
		}

		currentScene = nextScene;
		nextScene = nullptr;

		currentScene->Load();
	}

	if (!currentScene)
	{
		currentScene->Update(deltaTime_);
	}
}

void SceneManager::FixedUpdate(float fixedDeltaTime_)
{
	if (!currentScene)
	{
		currentScene->FixedUpdate(fixedDeltaTime_);
	}
}

void SceneManager::Render()
{
	if (!currentScene)
	{
		return;
	}

	currentScene->Render();
}

void SceneManager::AddScene(std::wstring_view sceneName_, std::unique_ptr<Scene> scene_)
{
	scenes.emplace(sceneName_, std::move(scene_));
}

void SceneManager::RemoveScene(std::wstring_view sceneName_)
{
	const auto it = scenes.find(std::wstring(sceneName_));
	if (it != scenes.end())
	{
		if (currentScene == it->second.get())
		{
			currentScene->Unload();
			currentScene = nullptr;
		}

		if (nextScene == it->second.get())
		{
			nextScene = nullptr;
		}

		scenes.erase(it);
	}
}

void SceneManager::LoadScene(std::wstring_view sceneName_)
{
	const auto it = scenes.find(std::wstring(sceneName_));
	if (it != scenes.end())
	{
		nextScene = it->second.get();
	}
}

void SceneManager::UnloadScene(std::wstring_view sceneName_)
{
	const auto it = scenes.find(std::wstring(sceneName_));
	if (it != scenes.end())
	{
		if (currentScene == it->second.get())
		{
			currentScene->Unload();
			currentScene = nullptr;
		}

		if (nextScene == it->second.get())
		{
			nextScene = nullptr;
		}
	}
}
