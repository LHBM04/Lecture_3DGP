#pragma once

#include "Object.hpp"

class Camera;

class Scene
{
public:
	virtual ~Scene() = default;

	[[nodiscard]] const std::vector<Camera*>& GetCameras() const;
	void AddCamera(Camera* camera_);
	void RemoveCamera(Camera* camera_);

	[[nodiscard]] const std::vector<std::unique_ptr<Object>>& GetObjects() const;
	Object* AddObject();
	void RemoveObject(Object* object_);

	void Load();
	void Update();
	void Render();
	void Unload();

protected:
	virtual void OnLoad() {}
	virtual void OnUpdate() {}
	virtual void OnRender() {}
	virtual void OnUnload() {}

private:
	std::vector<Camera*> cameras;
	std::vector<std::unique_ptr<Object>> objects;
};