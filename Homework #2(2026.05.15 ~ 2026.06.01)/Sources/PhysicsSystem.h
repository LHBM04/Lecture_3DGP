#pragma once

#include <memory>
#include <set>
#include <string_view>
#include <vector>

#include <DirectXCollision.h>

#include "Singleton.h"
#include "Vector3D.h"

class Collider;
class GameObject;

class PhysicsSystem final : public Singleton<PhysicsSystem>
{
	friend class Singleton<PhysicsSystem>;

public:
	PhysicsSystem();
	~PhysicsSystem() override = default;

	void Update(float fixedDeltaTime_);

	void AddCollider(Collider* collider_);
	void RemoveCollider(Collider* collider_);

	[[nodiscard]] bool IsCollidingWithStatic(Collider* collider_) const;
	[[nodiscard]] std::vector<Collider*> GetNearbyStaticColliders(Collider* collider_) const;
	[[nodiscard]] GameObject* Raycast(const Vector3D& rayOrigin_, const Vector3D& rayDir_, float* distance_ = nullptr) const;

	void RegisterStaticObjectsToGrid();

	void Clear();

private:
	void ProcessPhysics(float fixedDeltaTime_);
	void CreateGrid();
	void RemoveColliderReferences(Collider* collider_);

private:
	struct ColliderPair
	{
		Collider* c1;
		Collider* c2;

		bool operator<(const ColliderPair& other) const
		{
			if (c1 != other.c1) return c1 < other.c1;
			return c2 < other.c2;
		}
	};

	struct Cell
	{
		std::vector<Collider*> staticColliders;
		std::vector<Collider*> dynamicColliders;
	};

	std::vector<Collider*> colliders;
	std::set<ColliderPair> previousCollisions;

	// Grid System
	std::vector<std::vector<Cell>> grid;
	Vector3D gridOrigin{ -100.0f, 0.0f, -100.0f };
	float cellSize{ 10.0f };
	int gridWidth{ 20 };
	int gridHeight{ 20 };

	bool isGridInitialized{ false };
};
