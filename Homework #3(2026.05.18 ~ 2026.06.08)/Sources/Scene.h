#pragma once

#include <memory>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "GameObject.h"
#include "Vector3D.h"

class Camera;
class Collider;
class GameObject;
class Light;
class Transform;
class Quaternion;

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

	void Update();
	void LateUpdate();
	void FixedUpdate();
		
	void Render();

	GameObject* Instantiate();
	GameObject* Instantiate(const Vector3D& position_, const Quaternion& rotation_);
	GameObject* Instantiate(const Vector3D& position_);
	GameObject* Instantiate(const Quaternion& rotation_);
	GameObject* Instantiate(Transform* parent_);
	GameObject* Instantiate(const Vector3D& position_, const Quaternion& rotation_, Transform* parent_);

	GameObject* Instantiate(const class Model* model_);
	GameObject* Instantiate(const class Model* model_, const Vector3D& position_, const Quaternion& rotation_);
	GameObject* Instantiate(const class Model* model_, const Vector3D& position_);
	GameObject* Instantiate(const class Model* model_, const Quaternion& rotation_);
	GameObject* Instantiate(const class Model* model_, Transform* parent_);
	GameObject* Instantiate(const class Model* model_, const Vector3D& position_, const Quaternion& rotation_, Transform* parent_);

	void Destroy(GameObject* gameObject_);

	void AddCamera(Camera* camera_);
	void RemoveCamera(Camera* camera_);

	void AddLight(Light* light_);
	void RemoveLight(Light* light_);

	[[nodiscard]] GameObject* FindObjectWithName(std::wstring_view name_);
	[[nodiscard]] std::vector<GameObject*> FindObjectsWithName(std::wstring_view name_);
	[[nodiscard]] std::vector<const GameObject*> FindObjectsWithName(std::wstring_view name_) const;

	[[nodiscard]] GameObject* FindObjectWithTag(std::wstring_view tag_);
	[[nodiscard]] std::vector<GameObject*> FindObjectsWithTag(std::wstring_view tag_);
	[[nodiscard]] std::vector<const GameObject*> FindObjectsWithTag(std::wstring_view tag_) const;

	[[nodiscard]] std::span<const std::unique_ptr<GameObject>> GetGameObjects() const noexcept;

	[[nodiscard]] std::span<Camera* const> GetCameras();
	[[nodiscard]] std::span<const Camera* const> GetCameras() const;
	
	[[nodiscard]] std::span<Light* const> GetLights();
	[[nodiscard]] std::span<const Light* const> GetLights() const;

protected:
	virtual void OnLoad() = 0;
	virtual void OnUnload() = 0;

private:
	void FlushPendingObjects();
	void FlushDestroyObjects();

protected:
	bool isLoaded{ false };

	std::vector<std::unique_ptr<GameObject>> gameObjects;
	std::vector<std::unique_ptr<GameObject>> addObjects;
	std::vector<GameObject*> destroyObjects;
	std::vector<Camera*> cameras;
	std::vector<Light*> lights;
};
