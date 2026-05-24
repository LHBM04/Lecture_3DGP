#include "Precompiled.h"
#include "SceneManager.h"

#include "Scene.h"

namespace
{
	std::unordered_map<std::wstring, std::unique_ptr<Scene>> scenes;
	Scene* currentScene;
}

void SceneManager::Update()
{
	if (nullptr == currentScene)
	{
		return;
	}

	currentScene->Update();
}

void SceneManager::Render()
{
	if (nullptr == currentScene)
	{
		return;
	}

	currentScene->Render();
}

void SceneManager::AddScene(const std::wstring& name_, std::unique_ptr<Scene> scene_)
{
	if (scenes.contains(name_) || nullptr == scene_)
	{
		return;
	}

	scenes.emplace(name_, std::move(scene_));
}

void SceneManager::RemoveScene(const std::wstring& name_)
{
	auto iterator{ scenes.find(name_) };
	if (scenes.end() == iterator)
	{
		return;
	}

	if (currentScene == iterator->second.get())
	{
		currentScene->Unload();
		currentScene = nullptr;
	}

	scenes.erase(iterator);
}

void SceneManager::LoadScene(std::size_t index_) noexcept
{
	
}

void SceneManager::LoadScene(const std::wstring& name_) noexcept
{
	auto iterator{ scenes.find(name_) };
	if (scenes.end() == iterator)
	{
		return;
	}

	if (nullptr != currentScene)
	{
		currentScene->Unload();
	}

	currentScene = iterator->second.get();
	currentScene->Load();
}

void SceneManager::UnloadScene(std::size_t index) noexcept
{

}

void SceneManager::UnloadScene(const std::wstring& name_) noexcept
{
	auto iterator{ scenes.find(name_) };
	if (scenes.end() == iterator)
	{
		return;
	}

	iterator->second->Unload();
	if (currentScene == iterator->second.get())
	{
		currentScene = nullptr;
	}
}

Scene* SceneManager::GetCurrentScene() noexcept
{
	return currentScene;
}
