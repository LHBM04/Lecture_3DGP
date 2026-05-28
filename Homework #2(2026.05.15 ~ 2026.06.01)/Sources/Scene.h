#pragma once

#include "GameObject.h"

class LoadContext;
class TimeContext;
class RenderContext;

class Camera;
class Light;

class Scene
{
public:
	Scene() = default;
	~Scene() = default;

	// 복사 금지.
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	// 이동 금지.
	Scene(Scene&&) = delete;
	Scene& operator=(Scene&&) = delete;

	void Load(const LoadContext& context_);
	void Update(const TimeContext&  context_);
	void Render(RenderContext& context_);
	void Unload(const LoadContext& context_);

protected:
	virtual void OnLoad(const LoadContext& context_) {}
	virtual void OnUpdate(const TimeContext& context_) {}
	virtual void OnRender(RenderContext& context_) {}
	virtual void OnUnload(const LoadContext& context_) {}
	
private:
	std::vector<std::unique_ptr<GameObject>> gameObjects;
	std::vector<Camera*> cameras;
	std::vector<Light*> lights;
};
