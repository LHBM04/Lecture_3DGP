#pragma once

#include <memory>
#include <span>
#include <vector>

#include "GameObject.h"

class Camera;
class Light;

class Scene
{
public:
	Scene() = default;
	virtual ~Scene() = default;

	// 복사 금지.
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	// 이동 금지.
	Scene(Scene&&) = delete;
	Scene& operator=(Scene&&) = delete;

	void Load();
	void Unload();

	void Update(float deltaTime_);
	void FixedUpdate(float fixedDeltaTime_);
		
	void Render();

	GameObject* CreateGameObject();

	void AddCamera(Camera* camera_);
	void RemoveCamera(Camera* camera_);

	void AddLight(Light* light_);
	void RemoveLight(Light* light_);

	[[nodiscard]] std::span<Camera* const> GetCameras() noexcept;
	[[nodiscard]] std::span<const Camera* const> GetCameras() const noexcept;
	
	[[nodiscard]] std::span<Light* const> GetLights() noexcept;
	[[nodiscard]] std::span<const Light* const> GetLights() const noexcept;

protected:
	virtual void OnLoad() = 0;
	virtual void OnUnload() = 0;
	
private:
	bool isLoaded{ false };

	std::vector<std::unique_ptr<GameObject>> gameObjects;
	std::vector<Camera*> cameras;
	std::vector<Light*> lights;
};

inline void Scene::AddCamera(Camera* camera_)
{
	cameras.push_back(camera_);
}

inline void Scene::RemoveCamera(Camera* camera_)
{
	cameras.erase(std::remove(cameras.begin(), cameras.end(), camera_), cameras.end());
}

inline void Scene::AddLight(Light* light_)
{
	lights.push_back(light_);
}

inline void Scene::RemoveLight(Light* light_)
{
	lights.erase(std::remove(lights.begin(), lights.end(), light_), lights.end());
}

inline std::span<Camera* const> Scene::GetCameras() noexcept
{
	return cameras;
}

inline std::span<const Camera* const> Scene::GetCameras() const noexcept
{
	return cameras;
}

inline std::span<Light* const> Scene::GetLights() noexcept
{
	return lights;
}

inline std::span<const Light* const> Scene::GetLights() const noexcept
{
	return lights;
}
