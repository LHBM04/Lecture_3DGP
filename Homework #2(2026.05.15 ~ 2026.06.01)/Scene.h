#pragma once

#include "GameObject.h"

class Camera;
class Renderer;
class UIComponent;

class Scene
{
public:
	virtual ~Scene() noexcept = default;

	Scene() noexcept = default;

	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	Scene(Scene&&) = delete;
	Scene& operator=(Scene&&) = delete;

	void AddCamera(Camera* const camera_);
	void RemoveCamera(Camera* const camera_);
	void SetMainCamera(Camera* camera_);

	[[nodiscard]] Camera* GetMainCamera() noexcept;
	[[nodiscard]] const Camera* GetMainCamera() const noexcept;
	[[nodiscard]] const std::vector<Camera*>& GetCameras() const noexcept;

	[[nodiscard]] GameObject* AddGameObject();
	[[nodiscard]] GameObject* AddUIGameObject();
	void RemoveGameObject(GameObject* const gameObject_);
	void AddUIComponent(UIComponent* const uiComponent_);
	void RemoveUIComponent(UIComponent* const uiComponent_);

	void Load();
	void Update();
	void Render(Renderer& renderer_);
	void Unload();

protected:
	virtual void OnLoad() {}
	virtual void OnUpdate() {}
	virtual void OnRender(Renderer& renderer_) {}
	virtual void OnUnload() {}

private:
	void RenderInstancedMeshes(Renderer& renderer_);

	bool isLoaded{ false };
	Camera* mainCamera{ nullptr };
	std::vector<Camera*> cameras;
	std::vector<UIComponent*> uiComponents;
	std::vector<std::unique_ptr<GameObject>> objects;
};
