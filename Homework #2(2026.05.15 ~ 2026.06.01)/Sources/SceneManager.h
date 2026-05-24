#pragma once

#include "Singleton.h"
#include "Scene.h"

#include <memory>
#include <vector>

class SceneManager final : public Singleton<SceneManager>
{
	friend class Singleton<SceneManager>;

private:
	SceneManager() = default;
	~SceneManager() = default;

public:
	bool Initialize();
	void Release();

	void ChangeScene(std::unique_ptr<Scene> scene_);
	void Update();
	void Render();

	[[nodiscard]] Scene* GetCurrentScene() noexcept;
	[[nodiscard]] const Scene* GetCurrentScene() const noexcept;

private:
	void ApplyPendingScene();

	std::unique_ptr<Scene> currentScene;
	std::unique_ptr<Scene> pendingScene;
};
