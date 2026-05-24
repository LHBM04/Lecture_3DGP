#include "Precompiled.h"
#include "SceneManager.h"

bool SceneManager::Initialize()
{
	ApplyPendingScene();
	return true;
}

void SceneManager::Release()
{
	if (currentScene)
	{
		currentScene->Unload();
		currentScene.reset();
	}

	pendingScene.reset();
}

void SceneManager::ChangeScene(std::unique_ptr<Scene> scene_)
{
	pendingScene = std::move(scene_);
}

void SceneManager::Update()
{
	ApplyPendingScene();

	if (currentScene)
	{
		currentScene->Update();
	}

	ApplyPendingScene();
}

void SceneManager::Render()
{
	if (nullptr != currentScene)
	{
		currentScene->Render();
	}
}


Scene* SceneManager::GetCurrentScene() noexcept
{
	return currentScene.get();
}

const Scene* SceneManager::GetCurrentScene() const noexcept
{
	return currentScene.get();
}

void SceneManager::ApplyPendingScene()
{
	if (!pendingScene)
	{
		return;
	}

	if (currentScene)
	{
		currentScene->Unload();
	}

	currentScene = std::move(pendingScene);
	currentScene->Load();
}
