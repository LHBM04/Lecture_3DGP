#pragma once

#include <memory>
#include <set>
#include <span>
#include <utility>
#include <vector>

#include "Vector3D.h"

class Camera;
class GameObject;
class Light;

class Scene
{
public:
	Scene() = default;
	virtual ~Scene() = default;

	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

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

	[[nodiscard]] GameObject* Pick(const Vector3D& rayOrigin_, const Vector3D& rayDir_, float* distance_ = nullptr);

	[[nodiscard]] std::span<Camera* const> GetCameras();
	[[nodiscard]] std::span<const Camera* const> GetCameras() const;
	
	[[nodiscard]] std::span<Light* const> GetLights();
	[[nodiscard]] std::span<const Light* const> GetLights() const;

protected:
	virtual void OnLoad() = 0;
	virtual void OnUnload() = 0;
	
private:
	void ProcessPhysics(float fixedDeltaTime_);

private:
	bool isLoaded{ false };

	std::vector<std::unique_ptr<GameObject>> gameObjects;
	
	std::vector<Camera*> cameras;
	std::vector<Light*> lights;
};

inline std::span<Camera* const> Scene::GetCameras()
{
	return cameras;
}

inline std::span<const Camera* const> Scene::GetCameras() const
{
	return cameras;
}

inline std::span<Light* const> Scene::GetLights()
{
	return lights;
}

inline std::span<const Light* const> Scene::GetLights() const
{
	return lights;
}
