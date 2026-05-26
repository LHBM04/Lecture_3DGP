#include "Precompiled.h"
#include "SceneSystem.h"

#include "InputContext.h"
#include "Logger.h"
#include "RenderContext.h"

SceneSystem::~SceneSystem() noexcept
{
	SceneSystem::Release();
}

bool SceneSystem::Initialize(const SceneOptions& options_)
{
	for (const SceneBuildEntry& entry : options_.scenes)
	{
		if (entry.name.empty() || entry.create == nullptr)
		{
			Release();
			return false;
		}

		if (forName.contains(entry.name))
		{
			Release();
			return false;
		}

		std::unique_ptr<Scene> scene = entry.create();
		if (scene == nullptr)
		{
			Release();
			return false;
		}

		const std::size_t buildIndex = scenes.size();
		forName.emplace(entry.name, buildIndex);
		scenes.emplace_back(std::move(scene));
	}

	LoadScene(options_.startIndex);
	return true;
}

void SceneSystem::Release()
{
	if (nullptr != currentScene)
	{
		currentScene->Unload();
	}

	currentScene = nullptr;
	nextScene = nullptr;
	sceneContext.ClearSceneChangeRequest();

	forName.clear();
	scenes.clear();
}

void SceneSystem::Update(const TimeContext& context_)
{
	if (nullptr != nextScene)
	{
		if (nullptr != currentScene)
		{
			currentScene->Unload();
		}

		currentScene = nextScene;
		nextScene = nullptr;
		currentScene->Load(sceneContext);
	}

	if (nullptr == currentScene)
	{
		Logger::Critical("No Current Scene!!");
		return;
	}

	sceneContext.ClearSceneChangeRequest();
	currentScene->Update(context_);

	if (sceneContext.HasSceneChangeRequest())
	{
		LoadScene(sceneContext.GetRequestedSceneName());
		sceneContext.ClearSceneChangeRequest();
	}
}

void SceneSystem::FixedUpdate(const TimeContext& context_)
{
	if (nullptr == currentScene)
	{
		return;
	}

	currentScene->FixedUpdate(context_);
}

void SceneSystem::Render(RenderContext& context_)
{
	if (nullptr == currentScene)
	{
		Logger::Critical("No Current Scene!!");
		return;
	}

	currentScene->Render(context_);
}

void SceneSystem::DispatchInput(const InputContext& context_)
{
	if (nullptr != currentScene)
	{
		currentScene->DispatchInput(context_);
	}
}

void SceneSystem::LoadScene(std::size_t index_)
{
	if (index_ >= scenes.size())
	{
		return;
	}

	nextScene = scenes[index_].get();
}

void SceneSystem::LoadScene(std::wstring name_)
{
	const auto iter = forName.find(name_);
	if (iter == forName.end())
	{
		return;
	}

	LoadScene(iter->second);
}

void SceneSystem::UnloadScene()
{
	if (nullptr != currentScene)
	{
		currentScene->Unload();
	}
}

Scene* SceneSystem::GetCurrentScene() const noexcept
{
	return currentScene;
}
