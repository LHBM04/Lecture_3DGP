#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Scene.h"
#include "Singleton.h"

struct ID3D12GraphicsCommandList;

class SceneSystem final : public Singleton<SceneSystem>
{
public:
	SceneSystem() = default;
	~SceneSystem() override;

	template <std::derived_from<Scene> TScene, class... TArgs>
	bool AddScene(std::wstring_view sceneName_, TArgs&&... args_);

	template <std::derived_from<Scene> TScene>
	bool RemoveScene(std::wstring_view sceneName_);

	bool LoadScene(std::wstring_view sceneName_);
	void UnloadScene();

	void Update();
	void LateUpdate();
	void FixedUpdate();
	void Render(ID3D12GraphicsCommandList* commandList_);

	[[nodiscard]] Scene* GetCurrentScene() noexcept;
	[[nodiscard]] const Scene* GetCurrentScene() const noexcept;

private:
	bool AddSceneInternal(std::wstring_view sceneName_, std::unique_ptr<Scene> scene_);
	bool RemoveSceneInternal(std::wstring_view sceneName_);

	std::unordered_map<std::wstring, std::unique_ptr<Scene>> scenes;

	Scene* nextScene{ nullptr };
	Scene* currentScene{ nullptr };
};

template <std::derived_from<Scene> TScene, class... TArgs>
inline bool SceneSystem::AddScene(std::wstring_view sceneName_, TArgs&&... args_)
{
	return AddSceneInternal(sceneName_, std::make_unique<TScene>(std::forward<TArgs>(args_)...));
}

template <std::derived_from<Scene> TScene>
inline bool SceneSystem::RemoveScene(std::wstring_view sceneName_)
{
	const auto iter{ scenes.find(std::wstring(sceneName_)) };
	if (iter == scenes.end())
	{
		return false;
	}

	if (dynamic_cast<TScene*>(iter->second.get()) == nullptr)
	{
		return false;
	}

	return RemoveSceneInternal(sceneName_);
}
