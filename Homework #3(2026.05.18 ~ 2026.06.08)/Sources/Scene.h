#pragma once

#include <memory>
#include <vector>
#include <string_view>

#include "GameObject.h"
#include "Quaternion.h"
#include "Vector3D.h"

class Transform;
struct ID3D12GraphicsCommandList;
class Camera;
class Light;

class Scene
{
public:
	Scene() = default;
	virtual ~Scene() = default;

	void Load();
	void Unload();

	void Update();
	void LateUpdate();
	void FixedUpdate();
	
	void Render(ID3D12GraphicsCommandList* commandList_);

	GameObject* Instantiate();
	GameObject* Instantiate(const Vector3D& position_, const Quaternion& rotation_);
	GameObject* Instantiate(const Vector3D& position_);
	GameObject* Instantiate(const Quaternion& rotation_);
	GameObject* Instantiate(Transform* parent_);
	GameObject* Instantiate(const Vector3D& position_, const Quaternion& rotation_, Transform* parent_);

	void AddCamera(Camera* camera_);
	void RemoveCamera(Camera* camera_);
	void AddLight(Light* light_);
	void RemoveLight(Light* light_);

	[[nodiscard]] const std::vector<Camera*>& GetCameras() const noexcept;
	[[nodiscard]] const std::vector<Light*>& GetLights() const noexcept;

	[[nodiscard]] GameObject* FindObjectWithName(std::wstring_view name_);
	[[nodiscard]] auto FindObjectsWithName(std::wstring_view name_);

	[[nodiscard]] GameObject* FindObjectWithTag(std::wstring_view tag_);
	[[nodiscard]] auto FindObjectsWithTag(std::wstring_view tag_);

protected:
	virtual void OnLoad() {};
	virtual void OnUnload() {};

	virtual void OnUpdate() {};
	virtual void OnLateUpdate() {};
	virtual void OnFixedUpdate() {};

	virtual void OnRender(ID3D12GraphicsCommandList* commandList_) {}

private:
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	Scene(Scene&&) = delete;
	Scene& operator=(Scene&&) = delete;
	
	bool isLoaded{ false };

	std::vector<std::unique_ptr<GameObject>> gameObjects;
	std::vector<GameObject*> addQueue;
	std::vector<GameObject*> destroyQueue;
	std::vector<Camera*> cameras;
	std::vector<Light*> lights;
};
