#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Singleton.h"

class Scene;

class SceneSystem final : public Singleton<SceneSystem>
{
public:
	SceneSystem() = default;
	~SceneSystem() override = default;

	void AddScene();
	void RemoveScene();

	void LoadScene();
	void UnloadScene();

	[[nodiscard]] Scene* GetCurrentScene() noexcept;
	[[nodiscard]] const Scene* GetCurrentScene() const noexcept;

private:
	std::unordered_map<std::wstring, std::unique_ptr<Scene>> scenes;

	Scene* nextScene{ nullptr };
	Scene* currentScene{ nullptr };
};
