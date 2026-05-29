#include "Precompiled.h"
#include "SceneManager.h"

#include "RenderSystem.h"

void SceneManager::Update(float deltaTime_)
{
	if (currentScene)
	{
		// currentScene->Update(deltaTime_);
	}
}

void SceneManager::FixedUpdate(float fixedDeltaTime_)
{
	if (currentScene)
	{
		// currentScene->FixedUpdate(fixedDeltaTime_);
	}
}

void SceneManager::Render()
{
	if (!currentScene)
	{
		return;
	}

	for (Camera* camera : currentScene->GetCameras())
	{
	}
}
