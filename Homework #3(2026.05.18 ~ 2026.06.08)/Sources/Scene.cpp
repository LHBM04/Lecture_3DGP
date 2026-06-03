#include "Precompiled.h"
#include "Scene.h"

void Scene::Update(const TimeContext& time_)
{
	(void)time_;
}

void Scene::Render(RenderContext& renderContext_)
{
	(void)renderContext_;
}

ApplicationRequest* Scene::GetApplicationRequest() const noexcept
{
	return applicationRequest;
}

SceneRequest* Scene::GetSceneRequest() const noexcept
{
	return sceneRequest;
}

void Scene::SetApplicationRequest(ApplicationRequest* applicationRequest_) noexcept
{
	applicationRequest = applicationRequest_;
}

void Scene::SetSceneRequest(SceneRequest* sceneRequest_) noexcept
{
	sceneRequest = sceneRequest_;
}
