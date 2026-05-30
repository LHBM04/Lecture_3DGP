#include "Precompiled.h"
#include "SceneSystem.h"

#include "RenderSystem.h"

void SceneSystem::Update(float deltaTime_)
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

	if (currentScene)
	{
		currentScene->Update(deltaTime_);
	}
}

void SceneSystem::FixedUpdate(float fixedDeltaTime_)
{
	if (currentScene)
	{
		currentScene->FixedUpdate(fixedDeltaTime_);
	}
}

void SceneSystem::LateUpdate(float deltaTime_)
{
	if (currentScene)
	{
		currentScene->LateUpdate(deltaTime_);
	}
}

void SceneSystem::Render()
{
	if (!currentScene)
	{
		return;
	}

	currentScene->Render();
}

void SceneSystem::AddScene(std::wstring_view sceneName_, std::unique_ptr<Scene> scene_)
{
	scenes.emplace(sceneName_, std::move(scene_));
}

void SceneSystem::RemoveScene(std::wstring_view sceneName_)
{
	const std::unordered_map<std::wstring, std::unique_ptr<Scene>>::iterator it = scenes.find(std::wstring(sceneName_));
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

void SceneSystem::LoadScene(std::wstring_view sceneName_)
{
	const std::unordered_map<std::wstring, std::unique_ptr<Scene>>::iterator it = scenes.find(std::wstring(sceneName_));
	if (it != scenes.end())
	{
		nextScene = it->second.get();
	}
}

void SceneSystem::UnloadScene(std::wstring_view sceneName_)
{
	const std::unordered_map<std::wstring, std::unique_ptr<Scene>>::iterator it = scenes.find(std::wstring(sceneName_));
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

