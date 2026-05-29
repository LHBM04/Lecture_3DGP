#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Scene.h"
#include "Singleton.h"

class SceneSystem final : public Singleton<SceneSystem>
{
public:
	SceneSystem() = default;
	~SceneSystem() override = default;

	void Update(float deltaTime_);
	void FixedUpdate(float fixedDeltaTime_);
	void Render();

	void AddScene(std::wstring_view sceneName_, std::unique_ptr<Scene> scene_);
	void RemoveScene(std::wstring_view sceneName_);

	void LoadScene(std::wstring_view sceneName_);
	void UnloadScene(std::wstring_view sceneName_);

private:
	std::unordered_map<std::wstring, std::unique_ptr<Scene>> scenes;

	Scene* nextScene{ nullptr };
	Scene* currentScene{ nullptr };
};
