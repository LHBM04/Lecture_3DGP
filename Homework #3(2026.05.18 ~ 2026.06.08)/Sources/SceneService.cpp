#include "Precompiled.h"
#include "SceneService.h"

void SceneService::AddScene(std::wstring_view name_, std::unique_ptr<Scene> scene_)
{
	if (scene_ == nullptr)
	{
		return;
	}

	scene_->SetApplicationRequest(applicationRequest);
	scene_->SetSceneRequest(sceneRequest);
	scenes[std::wstring(name_)] = std::move(scene_);
}

void SceneService::RemoveScene(std::wstring_view name_)
{
	std::wstring key{ name_ };
	auto it{ scenes.find(key) };
	if (it == scenes.end())
	{
		return;
	}

	if (currentScene == it->second.get())
	{
		currentScene = nullptr;
	}

	scenes.erase(it);
}

void SceneService::LoadScene(std::wstring_view name_)
{
	auto it{ scenes.find(std::wstring(name_)) };
	if (it == scenes.end())
	{
		return;
	}

	currentScene = it->second.get();
	currentScene->SetApplicationRequest(applicationRequest);
	currentScene->SetSceneRequest(sceneRequest);
}

void SceneService::UnloadScene()
{
	currentScene = nullptr;
}

void SceneService::Update(const TimeContext& time_)
{
	if (currentScene != nullptr)
	{
		currentScene->Update(time_);
	}
}

void SceneService::Render(RenderContext& renderContext_)
{
	if (currentScene != nullptr)
	{
		currentScene->Render(renderContext_);
	}
}

void SceneService::SetRequests(ApplicationRequest& applicationRequest_, SceneRequest& sceneRequest_) noexcept
{
	applicationRequest = &applicationRequest_;
	sceneRequest = &sceneRequest_;

	for (auto& [name, scene] : scenes)
	{
		(void)name;
		scene->SetApplicationRequest(applicationRequest);
		scene->SetSceneRequest(sceneRequest);
	}
}

void SceneService::SetSceneRequest(SceneRequest& sceneRequest_) noexcept
{
	sceneRequest = &sceneRequest_;
	for (auto& [name, scene] : scenes)
	{
		(void)name;
		scene->SetSceneRequest(sceneRequest);
	}
}

ApplicationRequest* SceneService::GetApplicationRequest() const noexcept
{
	return applicationRequest;
}

SceneRequest* SceneService::GetSceneRequest() const noexcept
{
	return sceneRequest;
}

void SceneService::OnAdd()
{
}

void SceneService::OnRemove()
{
	scenes.clear();
	applicationRequest = nullptr;
	sceneRequest = nullptr;
	currentScene = nullptr;
}
