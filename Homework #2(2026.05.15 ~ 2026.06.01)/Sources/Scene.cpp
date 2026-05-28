#include "Precompiled.h"
#include "Scene.h"

#include "LoadContext.h"
#include "RenderContext.h"
#include "TimeContext.h"

void Scene::Load(const LoadContext& context_)
{
	OnLoad(context_);
}

void Scene::Update(const TimeContext& context_)
{
	OnUpdate(context_);
	for (const auto& gameObject : gameObjects)
	{
		gameObject->Update(context_);
	}
}

void Scene::Render(const RenderContext& context_)
{
	OnRender(context_);

	for (const auto& camera : cameras)
	{
		// camera->Render(context_);
	}

	for (const auto& light : lights)
	{
		// light->Render(context_);
	}

	for (const auto& gameObject : gameObjects)
	{
		gameObject->Render(context_);
	}
}

void Scene::Unload(const LoadContext& context_)
{
	OnUnload(context_);
}
