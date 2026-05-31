#pragma once

#include "Vector3D.h"

class Camera;
class Collider;
class GameObject;
class Light;
class Quaternion;

class Scene
{
public:
	Scene();
	virtual ~Scene();

	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	Scene(Scene&&) = delete;
	Scene& operator=(Scene&&) = delete;

	void Load();
	void Unload();

	void Update(float deltaTime_);
	void FixedUpdate(float fixedDeltaTime_);
	void LateUpdate(float deltaTime_);
		
	void Render();

	GameObject* Instantiate();
	GameObject* Instantiate(const Vector3D& position_, const Quaternion& rotation_);
	void Destroy(GameObject* gameObject_);

	void AddCamera(Camera* camera_);
	void RemoveCamera(Camera* camera_);

	void AddLight(Light* light_);
	void RemoveLight(Light* light_);

	[[nodiscard]] GameObject* Pick(const Vector3D& rayOrigin_, const Vector3D& rayDir_, float* distance_ = nullptr);

	[[nodiscard]] GameObject* FindObjectWithName(std::wstring_view name_);
	[[nodiscard]] GameObject* FindObjectWithTag(std::wstring_view tag_);
	[[nodiscard]] std::vector<GameObject*> FindObjectsWithName(std::wstring_view name_);
	[[nodiscard]] std::vector<GameObject*> FindObjectsWithTag(std::wstring_view tag_);

	[[nodiscard]] const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const noexcept;

	[[nodiscard]] std::span<Camera* const> GetCameras();
	[[nodiscard]] std::span<const Camera* const> GetCameras() const;
	
	[[nodiscard]] std::span<Light* const> GetLights();
	[[nodiscard]] std::span<const Light* const> GetLights() const;

protected:
	virtual void OnLoad() = 0;
	virtual void OnUnload() = 0;

protected:
	bool isLoaded{ false };

	std::vector<std::unique_ptr<GameObject>> gameObjects;
	std::vector<Camera*> cameras;
	std::vector<Light*> lights;
	std::vector<GameObject*> destroyQueue;

	void ProcessDestroyQueue();
};
