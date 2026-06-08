#include "Precompiled.h"
#include "SceneSystem.h"

#include "PhysicsSystem.h"
#include "RenderSystem.h"

void SceneSystem::Release()
{
	if (currentScene != nullptr)
	{
		currentScene->Unload();
		currentScene = nullptr;
	}

	nextScene = nullptr;
	scenes.clear();
	quitRequested = false;
}

void SceneSystem::Update()
{
	if (nextScene != nullptr)
	{
		if (currentScene != nullptr)
		{
			currentScene->Unload();
		}

		currentScene = nextScene;
		nextScene = nullptr;

		PhysicsSystem::GetInstance().Clear();
		currentScene->Load();
	}

	if (currentScene != nullptr)
	{
		currentScene->Update();
		currentScene->LateUpdate();
	}
}

void SceneSystem::FixedUpdate()
{
	if (currentScene != nullptr)
	{
		currentScene->FixedUpdate();
	}
}

void SceneSystem::Render()
{
	if (currentScene == nullptr)
	{
		return;
	}

	RenderSystem::GetInstance().PreRender();
	RenderSystem::GetInstance().Clear();
	currentScene->Render();
	RenderSystem::GetInstance().Render();
	RenderSystem::GetInstance().PostRender();
	RenderSystem::GetInstance().Present();
}

void SceneSystem::AddScene(std::wstring_view sceneName_, std::unique_ptr<Scene> scene_)
{
	scenes.emplace(sceneName_, std::move(scene_));
}

void SceneSystem::RemoveScene(std::wstring_view sceneName_)
{
	const std::unordered_map<std::wstring, std::unique_ptr<Scene>>::iterator it{ scenes.find(std::wstring(sceneName_)) };
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
	const std::unordered_map<std::wstring, std::unique_ptr<Scene>>::iterator it{ scenes.find(std::wstring(sceneName_)) };
	if (it != scenes.end())
	{
		nextScene = it->second.get();
	}
}

void SceneSystem::UnloadScene(std::wstring_view sceneName_)
{
	const std::unordered_map<std::wstring, std::unique_ptr<Scene>>::iterator it{ scenes.find(std::wstring(sceneName_)) };
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

void SceneSystem::RequestQuit() noexcept
{
	quitRequested = true;
}

bool SceneSystem::IsQuitRequested() const noexcept
{
	return quitRequested;
}
