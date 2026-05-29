#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Scene.h"
#include "Singleton.h"

class SceneManager final : public Singleton<SceneManager>
{
public:
	SceneManager() = default;
	~SceneManager() override = default;

	void Update(float deltaTime_);
	void FixedUpdate(float fixedDeltaTime_);
	void Render();

	template <std::derived_from<Scene> TScene>
	void AddScene();

	void RemoveScene(std::wstring_view sceneName_);

	void LoadScene();
	void UnloadScene();

private:
	std::unordered_map<std::wstring, std::unique_ptr<Scene>> scenes;

	Scene* nextScene{ nullptr };
	Scene* currentScene{ nullptr };
};

template <std::derived_from<Scene> TScene>
inline void SceneManager::AddScene()
{
	const std::wstring sceneName{ typeid(TScene).name() };
	if (scenes.contains(sceneName))
	{
		return;
	}

	scenes.emplace(sceneName, std::make_unique<TScene>());
}
