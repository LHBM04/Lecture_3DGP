#include "Precompiled.hpp"
#include "SceneManager.hpp"

namespace TUK::Game
{
	namespace
	{
		SceneSubsystem* sceneSubsystem = nullptr;
		std::unordered_map<std::string, Scene*> scenesByName;

		SceneTransition* sceneTransition = nullptr;
		Delegate<Scene*>::ListenerId enterSceneListenerId = 0;

		[[nodiscard]] SceneTransition* GetTransition(Scene* scene)
		{
			if (sceneTransition && !sceneTransition->IsDestroyed())
			{
				return sceneTransition;
			}

			sceneTransition = nullptr;
			if (!scene)
			{
				return nullptr;
			}

			GameObject* transitionObject = scene->Instantiate();
			transitionObject->SetName("Scene Transition");
			transitionObject->SetDontDestroyOnLoad(true);

			sceneTransition = transitionObject->AddComponent<SceneTransition>();
			return sceneTransition;
		}
	}

	void SceneManager::Initialize(SceneSubsystem* subsystem) noexcept
	{
		ASSERT_FMT(subsystem, "월드 서브시스템이 nullptr입니다.");

		sceneSubsystem = subsystem;
		if (Scene* currentScene = sceneSubsystem->GetCurrentScene(); currentScene && currentScene->IsEntered())
		{
			[[maybe_unused]] SceneTransition* created = GetTransition(currentScene);
		}
		else
		{
			enterSceneListenerId = sceneSubsystem->OnEnterScene().AddListener([](Scene* enteredScene)
			{
				[[maybe_unused]] SceneTransition* created = GetTransition(enteredScene);
			});
		}
	}

	void SceneManager::Release() noexcept
	{
		if (sceneSubsystem && enterSceneListenerId != 0)
		{
			sceneSubsystem->OnEnterScene().RemoveListener(enterSceneListenerId);
			enterSceneListenerId = 0;
		}

		sceneTransition = nullptr;
		scenesByName.clear();
		sceneSubsystem = nullptr;
	}

	Scene* SceneManager::AddScene(std::string_view name, std::unique_ptr<Scene> scene)
	{
		if (!sceneSubsystem)
		{
			return nullptr;
		}

		Scene* addedScene = sceneSubsystem->AddScene(std::move(scene));
		if (addedScene && !name.empty())
		{
			scenesByName.insert_or_assign(std::string(name), addedScene);
		}

		return addedScene;
	}

	Scene* SceneManager::FindSceneByName(std::string_view name) noexcept
	{
		const auto result = scenesByName.find(std::string(name));
		return result != scenesByName.end() ? result->second : nullptr;
	}

	Scene* SceneManager::GetCurrentScene() noexcept
	{
		return sceneSubsystem ? sceneSubsystem->GetCurrentScene() : nullptr;
	}

	bool SceneManager::LoadScene(Scene* targetScene, f32 duration)
	{
		if (!sceneSubsystem)
		{
			return false;
		}

		if (duration <= 0.0f)
		{
			return sceneSubsystem->RequestSceneChange(targetScene);
		}

		SceneTransition* transition = GetTransition(sceneSubsystem->GetCurrentScene());
		return transition && transition->Go(targetScene, duration);
	}

	bool SceneManager::LoadScene(std::string_view name, f32 duration)
	{
		Scene* targetScene = FindSceneByName(name);
		if (!targetScene)
		{
			LOG_ERROR("등록되지 않은 씬입니다: {}", name);
			return false;
		}

		return LoadScene(targetScene, duration);
	}

	bool SceneManager::IsTransitioning() noexcept
	{
		return sceneTransition && sceneTransition->IsActive();
	}
}
