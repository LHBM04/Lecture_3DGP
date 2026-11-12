#include "Precompiled.hpp"
#include "SceneSubsystem.hpp"

#include <algorithm>

#include "Scene.hpp"

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	SceneSubsystem::~SceneSubsystem() noexcept
	{
		scenes.clear();
	}

	Scene* SceneSubsystem::AddScene(std::unique_ptr<Scene> scene)
	{
		ASSERT_FMT(scene, "scene이 nullptr입니다.");
		ASSERT_FMT(!currentScene, "currentScene가 이미 설정되어 있습니다.");

		Scene* result = scene.get();
		scenes.push_back(std::move(scene));
		return result;
	}

	bool SceneSubsystem::RequestSceneChange(Scene* targetScene) noexcept
	{
		if (!targetScene || targetScene == currentScene)
		{
			return false;
		}

		const bool isRegistered = std::ranges::any_of(scenes, [targetScene](const std::unique_ptr<Scene>& scene)
		{
			return scene.get() == targetScene;
		});
		if (!isRegistered)
		{
			return false;
		}

		nextScene = targetScene;
		return true;
	}

	void SceneSubsystem::ExitScene()
	{
		nextScene = nullptr;
		ExitCurrentScene();
		nextScene = nullptr;
	}

	void SceneSubsystem::PreRender(GPUCommandBuffer& commandBuffer)
	{
		if (currentScene)
		{
			currentScene->PreRender(commandBuffer);
		}
	}

	void SceneSubsystem::Render(GPUCommandBuffer& commandBuffer)
	{
		if (currentScene)
		{
			currentScene->Render(commandBuffer);
		}
	}

	void SceneSubsystem::PostRender(GPUCommandBuffer& commandBuffer)
	{
		if (currentScene)
		{
			currentScene->PostRender(commandBuffer);
		}
	}

	SystemStatus SceneSubsystem::OnStartup()
	{
		if (scenes.empty())
		{
			return SystemStatus::FAILURE;
		}

		currentScene = scenes.front().get();
		return SystemStatus::CONTINUE;
	}

	SystemStatus SceneSubsystem::OnEarlyUpdate()
	{
		if (nextScene)
		{
			if (nextScene != currentScene)
			{
				ExitCurrentScene();
			}
			currentScene = std::exchange(nextScene, nullptr);
		}

		if (!currentScene)
		{
			return SystemStatus::SUCCESS;
		}

		if (!currentScene->IsEntered())
		{
			Scene* enteringScene = currentScene;
			enteringScene->Enter();
			if (currentScene == enteringScene)
			{
				onEnterScene.Broadcast(enteringScene);
				NotifyIfSceneExited(enteringScene);
			}
		}

		if (currentScene)
		{
			Scene* updatingScene = currentScene;
			updatingScene->EarlyUpdate();
			NotifyIfSceneExited(updatingScene);
		}
		return SystemStatus::CONTINUE;
	}

	SystemStatus SceneSubsystem::OnFixedUpdate()
	{
		if (currentScene)
		{
			Scene* updatingScene = currentScene;
			updatingScene->FixedUpdate();
			NotifyIfSceneExited(updatingScene);
		}
		return SystemStatus::CONTINUE;
	}

	SystemStatus SceneSubsystem::OnUpdate()
	{
		if (currentScene)
		{
			Scene* updatingScene = currentScene;
			updatingScene->Update();
			NotifyIfSceneExited(updatingScene);
		}
		return SystemStatus::CONTINUE;
	}

	SystemStatus SceneSubsystem::OnLateUpdate()
	{
		if (currentScene)
		{
			Scene* updatingScene = currentScene;
			updatingScene->LateUpdate();
			NotifyIfSceneExited(updatingScene);
		}
		return currentScene || nextScene ? SystemStatus::CONTINUE : SystemStatus::SUCCESS;
	}

	void SceneSubsystem::OnShutdown()
	{
		ExitScene();
		onEnterScene.Clear();
		onExitScene.Clear();
		scenes.clear();
	}

	void SceneSubsystem::ExitCurrentScene()
	{
		Scene* exitingScene = std::exchange(currentScene, nullptr);
		if (exitingScene && exitingScene->IsEntered())
		{
			exitingScene->Exit();
			onExitScene.Broadcast(exitingScene);
		}
	}

	void SceneSubsystem::NotifyIfSceneExited(Scene* scene)
	{
		if (currentScene == scene && !scene->IsEntered())
		{
			currentScene = nullptr;
			onExitScene.Broadcast(scene);
		}
	}

}
