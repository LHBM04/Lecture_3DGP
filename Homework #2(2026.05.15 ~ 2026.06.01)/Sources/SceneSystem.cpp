#include "Precompiled.h"
#include "SceneSystem.h"

bool SceneSystem::Initialize()
{
	return true;
}

void SceneSystem::Release()
{
}

void SceneSystem::Update(const TimeContext& context_)
{
}

void SceneSystem::Render(RenderContext& context_)
{
	if (Scene* scene = GetCurrentScene())
	{
		scene->Render(context_);
	}
}

Scene* SceneSystem::GetCurrentScene() noexcept
{
	return nullptr;
}

const Scene* SceneSystem::GetCurrentScene() const noexcept
{
	return nullptr;
}
