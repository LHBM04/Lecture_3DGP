#include "Precompiled.h"
#include "SceneManager.h"

namespace
{
	std::unordered_map<std::string, std::unique_ptr<Scene>> sceneLists;
	Scene* currentScene{ nullptr };
}

void SceneManager::Initialize()
{
}

void SceneManager::Update()
{
	if (nullptr != currentScene)
	{
		currentScene->Update();
	}
}

void SceneManager::Render(RenderTarget& renderTarget_)
{
	if (nullptr != currentScene)
	{
		currentScene->Render(renderTarget_);
	}
}

void SceneManager::LoadScene(std::size_t index_)
{
	auto it = sceneLists.begin();
	std::advance(it, index_);

	if (sceneLists.end() != it)
	{
		currentScene = it->second.get();
	}
}

void SceneManager::LoadScene(const std::string& name_)
{
	auto it = sceneLists.find(name_);
	if (it == sceneLists.end())
	{
		throw std::out_of_range("Scene name not found.");
	}

	LoadScene(std::move(it->second));
}

void SceneManager::UnloadScene()
{
	if (nullptr != currentScene)
	{
		currentScene->Unload();
		currentScene = nullptr;
	}
}
