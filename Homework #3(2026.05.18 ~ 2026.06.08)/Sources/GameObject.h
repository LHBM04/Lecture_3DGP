#pragma once

class Scene;

class GameObject
{
public:
	[[nodiscard]] Scene* GetScene() const noexcept;
	void SetScene(Scene* scene_) noexcept;

private:
	Scene* scene{ nullptr };
};
