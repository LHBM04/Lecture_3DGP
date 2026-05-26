#pragma once

#include "Renderer.h"
#include "GameObject.h"

class Camera;
class Light;

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
	void AddLight(Light* light_);
	void RemoveLight(Light* light_);

	GameObject& CreateGameObject(const std::string& name_ = "GameObject");

	void Load();
	void Update();
	void FixedUpdate();
	void Render();
	void Unload();

protected:
	virtual void OnLoad() {};
	virtual void OnUpdate() {};
	virtual void OnFixedUpdate() {};
	virtual void OnRender() {};
	virtual void OnUnload() {};

private:
	struct CollisionPair
	{
		const GameObject* first{ nullptr };
		const GameObject* second{ nullptr };

		bool operator==(const CollisionPair& other_) const noexcept
		{
			return first == other_.first && second == other_.second;
		}
	};

	struct CollisionPairHash
	{
		std::size_t operator()(const CollisionPair& pair_) const noexcept
		{
			const std::size_t h1{ std::hash<const GameObject*>{}(pair_.first) };
			const std::size_t h2{ std::hash<const GameObject*>{}(pair_.second) };
			return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6u) + (h1 >> 2u));
		}
	};

	void PickAtMouse();
	void DispatchCollisionEvents();
	void RemoveDestroyedGameObjects();

	friend class GameObject;

	bool isLoaded{ false };

	std::vector<std::unique_ptr<GameObject>> gameObjects;
	std::vector<Camera*> cameras;
	std::vector<Light*> lights;
	std::unordered_set<CollisionPair, CollisionPairHash> activeCollisionPairs;
};
