#include "Precompiled.h"

#include "StageSceneController.h"

#include <windows.h>

#include "GameObject.h"
#include "Scene.h"

void StageSceneController::OnStart()
{
	initialEnemyCount = CountAliveEnemies();
}

void StageSceneController::OnUpdate(float deltaTime_)
{
	if (clearTriggered)
	{
		return;
	}

	if (initialEnemyCount <= 0)
	{
		return;
	}

	if (CountAliveEnemies() == 0)
	{
		clearTriggered = true;
		MessageBoxW(nullptr, L"Stage Clear!", L"Stage Clear", MB_OK | MB_ICONINFORMATION);
		PostQuitMessage(0);
	}
}

int StageSceneController::CountAliveEnemies() const
{
	const GameObject* const owner{ GetOwner() };
	if (owner == nullptr)
	{
		return 0;
	}

	Scene* const scene{ owner->GetScene() };
	if (scene == nullptr)
	{
		return 0;
	}

	int count{ 0 };
	for (const std::unique_ptr<GameObject>& go : scene->GetGameObjects())
	{
		if (!go)
		{
			continue;
		}
		if (go->IsDestroyed() || !go->IsActive())
		{
			continue;
		}
		if (go->GetTag() == L"Enemy")
		{
			++count;
		}
	}
	return count;
}

