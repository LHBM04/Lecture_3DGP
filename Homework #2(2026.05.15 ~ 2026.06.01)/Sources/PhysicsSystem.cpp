#include "Precompiled.h"

#include "PhysicsSystem.h"

#include "Collider.h"
#include "GameObject.h"
#include "Transform.h"

PhysicsSystem::PhysicsSystem()
{
	CreateGrid();
}

void PhysicsSystem::Update(float fixedDeltaTime_)
{
	ProcessPhysics(fixedDeltaTime_);
}

void PhysicsSystem::AddCollider(Collider* collider_)
{
	if (collider_ == nullptr) return;

	if (std::find(colliders.begin(), colliders.end(), collider_) == colliders.end())
	{
		colliders.push_back(collider_);
		
		// If it's a static collider, register it immediately
		if (collider_->IsStatic())
		{
			RegisterStaticObjectsToGrid();
		}
	}
}

void PhysicsSystem::RemoveCollider(Collider* collider_)
{
	if (collider_ == nullptr) return;

	const bool wasStatic{ collider_->IsStatic() };
	std::vector<Collider*>::iterator it{ std::find(colliders.begin(), colliders.end(), collider_) };
	if (it != colliders.end())
	{
		colliders.erase(it);
	}

	RemoveColliderReferences(collider_);

	if (wasStatic)
	{
		RegisterStaticObjectsToGrid();
	}
}

void PhysicsSystem::RemoveColliderReferences(Collider* collider_)
{
	if (collider_ == nullptr)
	{
		return;
	}

	std::erase_if(previousCollisions,
		[collider_](const ColliderPair& pair)
		{
			return pair.c1 == collider_ || pair.c2 == collider_;
		});

	for (std::vector<Cell>& row : grid)
	{
		for (Cell& cell : row)
		{
			std::erase(cell.staticColliders, collider_);
			std::erase(cell.dynamicColliders, collider_);
		}
	}
}

bool PhysicsSystem::IsCollidingWithStatic(Collider* collider_) const
{
	if (collider_ == nullptr) return false;

	DirectX::BoundingBox aabb{ collider_->GetBoundingVolume() };

	int minX{ static_cast<int>((aabb.Center.x - aabb.Extents.x - gridOrigin.x) / cellSize) };
	int minZ{ static_cast<int>((aabb.Center.z - aabb.Extents.z - gridOrigin.z) / cellSize) };
	int maxX{ static_cast<int>((aabb.Center.x + aabb.Extents.x - gridOrigin.x) / cellSize) };
	int maxZ{ static_cast<int>((aabb.Center.z + aabb.Extents.z - gridOrigin.z) / cellSize) };

	for (int z{ minZ }; z <= maxZ; ++z)
	{
		for (int x{ minX }; x <= maxX; ++x)
		{
			if (x >= 0 && x < gridWidth && z >= 0 && z < gridHeight)
			{
				for (Collider* staticCol : grid[z][x].staticColliders)
				{
					if (staticCol == collider_) continue;
					GameObject* staticOwner{ staticCol != nullptr ? staticCol->GetOwner() : nullptr };
					if (staticOwner == nullptr || staticOwner->IsDestroyed() || !staticOwner->IsActive())
					{
						continue;
					}

					const std::wstring_view staticName{ staticOwner->GetName() };
					const bool blocksMovement{
						staticName.find(L"Wall") != std::wstring::npos ||
						staticName.find(L"Stair") != std::wstring::npos ||
						staticName.find(L"Floor") != std::wstring::npos
					};

					if (blocksMovement && collider_->IsIntersects(staticCol))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

std::vector<Collider*> PhysicsSystem::GetNearbyStaticColliders(Collider* collider_) const
{
	std::vector<Collider*> nearby;
	if (collider_ == nullptr)
	{
		return nearby;
	}

	DirectX::BoundingBox aabb{ collider_->GetBoundingVolume() };

	const int minX{ static_cast<int>((aabb.Center.x - aabb.Extents.x - gridOrigin.x) / cellSize) };
	const int minZ{ static_cast<int>((aabb.Center.z - aabb.Extents.z - gridOrigin.z) / cellSize) };
	const int maxX{ static_cast<int>((aabb.Center.x + aabb.Extents.x - gridOrigin.x) / cellSize) };
	const int maxZ{ static_cast<int>((aabb.Center.z + aabb.Extents.z - gridOrigin.z) / cellSize) };

	for (int z{ minZ }; z <= maxZ; ++z)
	{
		for (int x{ minX }; x <= maxX; ++x)
		{
			if (x < 0 || x >= gridWidth || z < 0 || z >= gridHeight)
			{
				continue;
			}

			for (Collider* staticCol : grid[z][x].staticColliders)
			{
				if (staticCol != nullptr)
				{
					GameObject* staticOwner{ staticCol->GetOwner() };
					if (staticOwner == nullptr || staticOwner->IsDestroyed() || !staticOwner->IsActive())
					{
						continue;
					}
					nearby.emplace_back(staticCol);
				}
			}
		}
	}

	return nearby;
}

GameObject* PhysicsSystem::Raycast(const Vector3D& rayOrigin_, const Vector3D& rayDir_, float* distance_) const
{
	GameObject* closestObject{ nullptr };
	float minDistance{ std::numeric_limits<float>::max() };

	for (Collider* col : colliders)
	{
		if (col == nullptr || col->GetOwner() == nullptr || col->GetOwner()->IsDestroyed() || !col->GetOwner()->IsActive()) continue;

		float distance{ 0.0f };
		if (col->IsIntersects(rayOrigin_, rayDir_, distance))
		{
			if (distance < minDistance)
			{
				minDistance = distance;
				closestObject = col->GetOwner();
			}
		}
	}

	if (distance_ != nullptr)
	{
		*distance_ = minDistance;
	}
	return closestObject;
}

void PhysicsSystem::Clear()
{
	colliders.clear();
	previousCollisions.clear();
	
	for (std::vector<Cell>& row : grid)
	{
		for (Cell& cell : row)
		{
			cell.staticColliders.clear();
			cell.dynamicColliders.clear();
		}
	}
}

void PhysicsSystem::ProcessPhysics(float fixedDeltaTime_)
{
	auto isColliderValid = [](const Collider* col) -> bool
	{
		return col != nullptr &&
			col->GetOwner() != nullptr &&
			!col->GetOwner()->IsDestroyed();
	};

	std::erase_if(colliders, [&](Collider* col) { return !isColliderValid(col); });
	std::erase_if(previousCollisions, [&](const ColliderPair& pair)
	{
		return !isColliderValid(pair.c1) || !isColliderValid(pair.c2);
	});

	// 1. Clear dynamic objects from grid and collect them
	for (std::vector<Cell>& row : grid)
	{
		for (Cell& cell : row)
		{
			cell.dynamicColliders.clear();
		}
	}
	
	std::vector<Collider*> dynamicColliders;
	for (Collider* col : colliders)
	{
		if (!isColliderValid(col) || !col->GetOwner()->IsActive()) continue;

		if (!col->IsStatic())
		{
			dynamicColliders.push_back(col);
			
			DirectX::BoundingBox aabb{ col->GetBoundingVolume() };
			
			int minX{ static_cast<int>((aabb.Center.x - aabb.Extents.x - gridOrigin.x) / cellSize) };
			int minZ{ static_cast<int>((aabb.Center.z - aabb.Extents.z - gridOrigin.z) / cellSize) };
			int maxX{ static_cast<int>((aabb.Center.x + aabb.Extents.x - gridOrigin.x) / cellSize) };
			int maxZ{ static_cast<int>((aabb.Center.z + aabb.Extents.z - gridOrigin.z) / cellSize) };

			for (int z{ minZ }; z <= maxZ; ++z)
			{
				for (int x{ minX }; x <= maxX; ++x)
				{
					if (x >= 0 && x < gridWidth && z >= 0 && z < gridHeight)
					{
						grid[z][x].dynamicColliders.push_back(col);
					}
				}
			}
		}
	}

	// 2. Collision detection using grid
	std::set<ColliderPair> currentCollisions;
	for (Collider* col1 : dynamicColliders)
	{
		DirectX::BoundingBox aabb{ col1->GetBoundingVolume() };
		
		int minX{ static_cast<int>((aabb.Center.x - aabb.Extents.x - gridOrigin.x) / cellSize) };
		int minZ{ static_cast<int>((aabb.Center.z - aabb.Extents.z - gridOrigin.z) / cellSize) };
		int maxX{ static_cast<int>((aabb.Center.x + aabb.Extents.x - gridOrigin.x) / cellSize) };
		int maxZ{ static_cast<int>((aabb.Center.z + aabb.Extents.z - gridOrigin.z) / cellSize) };

		for (int z{ minZ }; z <= maxZ; ++z)
		{
			for (int x{ minX }; x <= maxX; ++x)
			{
				if (x >= 0 && x < gridWidth && z >= 0 && z < gridHeight)
				{
					// Dynamic vs Dynamic
					for (Collider* col2 : grid[z][x].dynamicColliders)
					{
						if (reinterpret_cast<uintptr_t>(col1) <= reinterpret_cast<uintptr_t>(col2)) continue;
						if (col1->IsIntersects(col2))
						{
							currentCollisions.insert({col1, col2});
						}
					}
					// Dynamic vs Static
					for (Collider* col2 : grid[z][x].staticColliders)
					{
						if (col1->IsIntersects(col2))
						{
							uintptr_t ptr1{ reinterpret_cast<uintptr_t>(col1) };
							uintptr_t ptr2{ reinterpret_cast<uintptr_t>(col2) };
							currentCollisions.insert(ptr1 < ptr2 ? ColliderPair{col1, col2} : ColliderPair{col2, col1});
						}
					}
				}
			}
		}
	}
	
	// 3. Trigger callbacks
	for (const ColliderPair& pair : currentCollisions)
	{
		if (!isColliderValid(pair.c1) || !isColliderValid(pair.c2))
		{
			continue;
		}

		auto canNotify = [](Collider* col) -> bool
		{
			return col != nullptr &&
				col->GetOwner() != nullptr &&
				!col->GetOwner()->IsDestroyed();
		};

		if (previousCollisions.find(pair) != previousCollisions.end())
		{
			if (canNotify(pair.c1))
			{
				pair.c1->GetOwner()->NotifyCollisionStay(pair.c2);
			}
			if (canNotify(pair.c2))
			{
				pair.c2->GetOwner()->NotifyCollisionStay(pair.c1);
			}
		}
		else
		{
			if (canNotify(pair.c1))
			{
				pair.c1->GetOwner()->NotifyCollisionEnter(pair.c2);
			}
			if (canNotify(pair.c2))
			{
				pair.c2->GetOwner()->NotifyCollisionEnter(pair.c1);
			}
		}
	}

	for (const ColliderPair& pair : previousCollisions)
	{
		if (!isColliderValid(pair.c1) || !isColliderValid(pair.c2))
		{
			continue;
		}

		if (currentCollisions.find(pair) == currentCollisions.end())
		{
			if (pair.c1->GetOwner() != nullptr && !pair.c1->GetOwner()->IsDestroyed())
			{
				pair.c1->GetOwner()->NotifyCollisionExit(pair.c2);
			}
			if (pair.c2->GetOwner() != nullptr && !pair.c2->GetOwner()->IsDestroyed())
			{
				pair.c2->GetOwner()->NotifyCollisionExit(pair.c1);
			}
		}
	}

	auto isColliderRegistered = [this](const Collider* col) -> bool
	{
		return std::find(colliders.begin(), colliders.end(), col) != colliders.end();
	};

	std::erase_if(currentCollisions, [&](const ColliderPair& pair)
	{
		return !isColliderRegistered(pair.c1) || !isColliderRegistered(pair.c2);
	});

	previousCollisions = std::move(currentCollisions);
}

void PhysicsSystem::CreateGrid()
{
	grid.assign(gridHeight, std::vector<Cell>(gridWidth));
	isGridInitialized = true;
}

void PhysicsSystem::RegisterStaticObjectsToGrid()
{
	if (!isGridInitialized) return;

	for (std::vector<Cell>& row : grid)
	{
		for (Cell& cell : row)
		{
			cell.staticColliders.clear();
		}
	}

	for (Collider* col : colliders)
	{
		if (col == nullptr || col->GetOwner() == nullptr || col->GetOwner()->IsDestroyed() || !col->GetOwner()->IsActive()) continue;

		if (col->IsStatic())
		{
			DirectX::BoundingBox aabb{ col->GetBoundingVolume() };
			
			int minX{ static_cast<int>((aabb.Center.x - aabb.Extents.x - gridOrigin.x) / cellSize) };
			int minZ{ static_cast<int>((aabb.Center.z - aabb.Extents.z - gridOrigin.z) / cellSize) };
			int maxX{ static_cast<int>((aabb.Center.x + aabb.Extents.x - gridOrigin.x) / cellSize) };
			int maxZ{ static_cast<int>((aabb.Center.z + aabb.Extents.z - gridOrigin.z) / cellSize) };

			for (int z{ minZ }; z <= maxZ; ++z)
			{
				for (int x{ minX }; x <= maxX; ++x)
				{
					if (x >= 0 && x < gridWidth && z >= 0 && z < gridHeight)
					{
						grid[z][x].staticColliders.push_back(col);
					}
				}
			}
		}
	}
}
