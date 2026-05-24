#pragma once

#include "Renderer.h"
#include "GameObject.h"

class Camera;

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

	void AddCamera(Camera* camera_);
	void RemoveCamera(Camera* camera_);

	GameObject& CreateGameObject(const std::string& name_ = "GameObject");

	void Load();
	void Update();
	void Render();
	void Unload();

protected:
	virtual void OnLoad() {};
	virtual void OnUpdate() {};
	virtual void OnRender() {};
	virtual void OnUnload() {};

private:
	friend class GameObject;

	bool isLoaded{ false };

	std::vector<std::unique_ptr<GameObject>> gameObjects;
	std::vector<Camera*> cameras;
};
