#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "Vector3D.h"

class Camera;
class Collider;
class GameObject;
class RenderContext;
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

	void Render(const RenderContext& context_);

	GameObject& Instantiate();
	GameObject& Instantiate(const Vector3D& position_, const Quaternion& rotation_);
	GameObject& Instantiate(const Vector3D& position_);
	GameObject& Instantiate(const Quaternion& rotation_);
	GameObject& Instantiate(Transform* parent_);
	GameObject& Instantiate(const Vector3D& position_, const Quaternion& rotation_, Transform* parent_);

	void Destroy(GameObject& gameObject_);

	void AddCamera(Camera& camera_);
	void RemoveCamera(Camera& camera_);

	void AddLight(Light& light_);
	void RemoveLight(Light& light_);

	[[nodiscard]] GameObject* FindObjectWithName(std::wstring_view name_);
	[[nodiscard]] auto FindObjectsWithName(std::wstring_view name_);

	[[nodiscard]] GameObject* FindObjectWithTag(std::wstring_view tag_);
	[[nodiscard]] auto FindObjectsWithTag(std::wstring_view tag_);

	[[nodiscard]] std::span<const std::unique_ptr<GameObject>> GetGameObjects() const noexcept;

	[[nodiscard]] std::span<Camera* const> GetCameras() noexcept;
	[[nodiscard]] std::span<Camera* const> GetCameras() const noexcept;

	[[nodiscard]] std::span<Light* const> GetLights() noexcept;
	[[nodiscard]] std::span<Light* const> GetLights() const noexcept;

protected:
	virtual void OnLoad() {};
	virtual void OnUnload() {};

	virtual void OnUpdate() {};
	virtual void OnLateUpdate() {};
	virtual void OnFixedUpdate() {};

	virtual void OnRender(const RenderContext& context_) {}

private:
	bool isLoaded{ false };

	std::vector<std::unique_ptr<GameObject>> gameObjects;
	std::vector<GameObject*> destroyQueue;
	std::vector<Camera*> cameras;
	std::vector<Light*> lights;
};
