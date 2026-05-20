#pragma once

#include "Scene.h"

class Renderer;

class SceneManager final
{
	STATIC_CLASS(SceneManager);

public:
	static void LoadScene(std::unique_ptr<Scene> scene_);
	static void Update();
	static void Render(Renderer& renderer_);
	static void UnloadScene();

	[[nodiscard]] static Scene* GetCurrentScene() noexcept;

private:
	static inline std::unique_ptr<Scene> currentScene;
};
