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
public:
	PhysicsSystem() noexcept = default;
	~PhysicsSystem() noexcept override = default;

	void Reset();
	void Update();

	void RegisterCollider(Collider* collider_);
	void UnregisterCollider(Collider* collider_);
	void AddCollider(Collider* collider_);
	void RemoveCollider(Collider* collider_);

	[[nodiscard]] bool CheckCollision(Collider* collider_) const;
	[[nodiscard]] bool IsCollidingWithStatic(Collider* collider_) const;
	[[nodiscard]] std::vector<Collider*> GetNearbyStaticColliders(Collider* collider_) const;
	
	struct RaycastHit
	{
		Collider* collider{ nullptr };
		GameObject* gameObject{ nullptr };
		Vector3D point{ Vector3D::GetZero() };
		Vector3D normal{ Vector3D::GetUp() };
		float distance{ 0.0f };
	};

	[[nodiscard]] bool Raycast(const Vector3D& rayOrigin_, const Vector3D& rayDir_, RaycastHit& hitInfo_, float maxDistance_ = 1000.0f) const;
	[[nodiscard]] bool Raycast(const Vector3D& rayOrigin_, const Vector3D& rayDir_, float maxDistance_ = 1000.0f) const;
	[[nodiscard]] GameObject* Raycast(const Vector3D& rayOrigin_, const Vector3D& rayDir_, float* distance_ = nullptr) const;

	[[nodiscard]] std::vector<Collider*> OverlapBox(const Vector3D& center_, const Vector3D& halfExtents_) const;

	void RegisterStaticObjectsToGrid();
	void Clear();
	void ApplyPendingColliderChanges();

private:
	void ProcessPhysics();
	void RemoveColliderFromCollisionHistory(Collider* collider_);

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
	std::vector<Collider*> pendingAddColliders;
	std::vector<Collider*> pendingRemoveColliders;
	std::set<ColliderPair> previousCollisions;

	std::vector<std::vector<Cell>> grid;
	Vector3D gridOrigin{ -300.0f, 0.0f, -300.0f };
	float cellSize{ 10.0f };
	int gridWidth{ 60 };
	int gridHeight{ 60 };

	bool isGridInitialized{ false };
};
