#include "Precompiled.h"
#include "SceneSystem.h"

#include "Scene.h"

SceneSystem::~SceneSystem() = default;

bool SceneSystem::AddSceneInternal(std::wstring_view sceneName_, std::unique_ptr<Scene> scene_)
{
	if (sceneName_.empty() || scene_ == nullptr)
	{
		return false;
	}

	const std::wstring sceneName{ sceneName_ };
	if (scenes.contains(sceneName))
	{
		return false;
	}

	scenes.emplace(sceneName, std::move(scene_));
	return true;
}

bool SceneSystem::RemoveSceneInternal(std::wstring_view sceneName_)
{
	const auto iter{ scenes.find(std::wstring(sceneName_)) };
	if (iter == scenes.end())
	{
		return false;
	}

	if (iter->second.get() == currentScene)
	{
		UnloadScene();
		currentScene = nullptr;
	}

	if (iter->second.get() == nextScene)
	{
		nextScene = nullptr;
	}

	scenes.erase(iter);
	return true;
}

bool SceneSystem::LoadScene(std::wstring_view sceneName_)
{
	const auto iter{ scenes.find(std::wstring(sceneName_)) };
	if (iter == scenes.end())
	{
		return false;
	}

	nextScene = iter->second.get();
	return true;
}

void SceneSystem::UnloadScene()
{
	if (currentScene == nullptr)
	{
		return;
	}

	currentScene->Unload();
}

void SceneSystem::Update()
{
	if (nextScene != nullptr && nextScene != currentScene)
	{
		UnloadScene();
		currentScene = nextScene;
		currentScene->Load();
		nextScene = nullptr;
	}

	if (currentScene == nullptr)
	{
		return;
	}

	currentScene->Update();
}

void SceneSystem::LateUpdate()
{
	if (currentScene == nullptr)
	{
		return;
	}

	currentScene->LateUpdate();
}

void SceneSystem::FixedUpdate()
{
	if (currentScene == nullptr)
	{
		return;
	}

	currentScene->FixedUpdate();
}

void SceneSystem::Render(ID3D12GraphicsCommandList* commandList_)
{
	if (currentScene == nullptr || commandList_ == nullptr)
	{
		return;
	}

	currentScene->Render(commandList_);
}

Scene* SceneSystem::GetCurrentScene() noexcept
{
	return currentScene;
}

const Scene* SceneSystem::GetCurrentScene() const noexcept
{
	return currentScene;
}
