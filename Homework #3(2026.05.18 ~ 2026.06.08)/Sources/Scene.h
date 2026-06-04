#pragma once

#include <memory>
#include <vector>
#include <string_view>

#include "GameObject.h"
#include "Quaternion.h"
#include "Vector3D.h"

class Transform;

class Scene
{
public:
	Scene() = default;
	virtual ~Scene() = default;

	void Load();
	void Update();
	void LateUpdate();
	void FixedUpdate();
	void Render();
	void Unload();

	GameObject* Instantiate();
	GameObject* Instantiate(const Vector3D& position_, const Quaternion& rotation_);
	GameObject* Instantiate(const Vector3D& position_);
	GameObject* Instantiate(const Quaternion& rotation_);
	GameObject* Instantiate(Transform* parent_);
	GameObject* Instantiate(const Vector3D& position_, const Quaternion& rotation_, Transform* parent_);

	[[nodiscard]] GameObject* FindObjectWithName(std::wstring_view name_);
	[[nodiscard]] auto FindObjectsWithName(std::wstring_view name_);

	[[nodiscard]] GameObject* FindObjectWithTag(std::wstring_view tag_);
	[[nodiscard]] auto FindObjectsWithTag(std::wstring_view tag_);

private:
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	Scene(Scene&&) = delete;
	Scene& operator=(Scene&&) = delete;

	std::vector<std::unique_ptr<GameObject>> gameObjects;
	std::vector<GameObject*> addQueue;
	std::vector<GameObject*> destroyQueue;
};
