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
	isProcessingPhysics = true;
	ProcessPhysics(fixedDeltaTime_);
	isProcessingPhysics = false;
	ApplyPendingColliderChanges();
}

void PhysicsSystem::RegisterCollider(Collider* collider_)
{
	AddCollider(collider_);
}

void PhysicsSystem::UnregisterCollider(Collider* collider_)
{
	RemoveCollider(collider_);
}

void PhysicsSystem::AddCollider(Collider* collider_)
{
	if (collider_ == nullptr)
	{
		return;
	}
	if (isProcessingPhysics)
	{
		pendingAddColliders.emplace_back(collider_);
		return;
	}

	if (std::find(colliders.begin(), colliders.end(), collider_) == colliders.end())
	{
		colliders.emplace_back(collider_);
		
		if (collider_->IsStatic())
		{
			RegisterStaticObjectsToGrid();
		}
	}
}

void PhysicsSystem::RemoveCollider(Collider* collider_)
{
	if (collider_ == nullptr)
	{
		return;
	}
	if (isProcessingPhysics)
	{
		pendingRemoveColliders.emplace_back(collider_);
		return;
	}

	std::vector<Collider*>::iterator it{ std::find(colliders.begin(), colliders.end(), collider_) };
	if (it != colliders.end())
	{
		colliders.erase(it);
		RemoveColliderFromCollisionHistory(collider_);

		if (collider_->IsStatic())
		{
			RegisterStaticObjectsToGrid();
		}
	}
}

bool PhysicsSystem::CheckCollision(Collider* collider_) const
{
	return IsCollidingWithStatic(collider_);
}

bool PhysicsSystem::IsCollidingWithStatic(Collider* collider_) const
{
	if (collider_ == nullptr || collider_->GetOwner() == nullptr)
	{
		return false;
	}

	const DirectX::BoundingBox aabb{ collider_->GetBoundingVolume() };

	const int minX{ static_cast<int>((aabb.Center.x - aabb.Extents.x - gridOrigin.x) / cellSize) };
	const int minZ{ static_cast<int>((aabb.Center.z - aabb.Extents.z - gridOrigin.z) / cellSize) };
	const int maxX{ static_cast<int>((aabb.Center.x + aabb.Extents.x - gridOrigin.x) / cellSize) };
	const int maxZ{ static_cast<int>((aabb.Center.z + aabb.Extents.z - gridOrigin.z) / cellSize) };

	for (int z{ minZ }; z <= maxZ; ++z)
	{
		for (int x{ minX }; x <= maxX; ++x)
		{
			if (x >= 0 && x < gridWidth && z >= 0 && z < gridHeight)
			{
				for (Collider* staticCol : grid[z][x].staticColliders)
				{
					if (staticCol == nullptr || staticCol == collider_ || staticCol->GetOwner() == nullptr)
					{
						continue;
					}

					const std::wstring& name{ staticCol->GetOwner()->GetName() };
					if (name.find(L"Wall") != std::wstring::npos || name.find(L"Stair") != std::wstring::npos)
					{
						if (collider_->IsIntersects(staticCol))
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

bool PhysicsSystem::Raycast(const Vector3D& rayOrigin_, const Vector3D& rayDir_, RaycastHit& hitInfo_, float maxDistance_) const
{
	Collider* closestCollider{ nullptr };
	float minDistance{ maxDistance_ };

	for (Collider* col : colliders)
	{
		if (col == nullptr || col->GetOwner() == nullptr || col->GetOwner()->IsDestroyed() || col->GetOwner()->IsActive() == false)
		{
			continue;
		}

		float distance{ 0.0f };
		if (col->IsIntersects(rayOrigin_, rayDir_, distance) && distance <= minDistance)
		{
			minDistance = distance;
			closestCollider = col;
		}
	}

	if (closestCollider == nullptr)
	{
		return false;
	}

	hitInfo_.collider = closestCollider;
	hitInfo_.gameObject = closestCollider->GetOwner();
	hitInfo_.distance = minDistance;
	hitInfo_.point = rayOrigin_ + rayDir_.GetNormalized() * minDistance;
	hitInfo_.normal = Vector3D::GetUp();

	return true;
}

bool PhysicsSystem::Raycast(const Vector3D& rayOrigin_, const Vector3D& rayDir_, float maxDistance_) const
{
	RaycastHit hitInfo;
	return Raycast(rayOrigin_, rayDir_, hitInfo, maxDistance_);
}

GameObject* PhysicsSystem::Raycast(const Vector3D& rayOrigin_, const Vector3D& rayDir_, float* distance_) const
{
	RaycastHit hitInfo;
	if (Raycast(rayOrigin_, rayDir_, hitInfo) == false)
	{
		if (distance_ != nullptr)
		{
			*distance_ = std::numeric_limits<float>::max();
		}
		return nullptr;
	}

	if (distance_ != nullptr)
	{
		*distance_ = hitInfo.distance;
	}
	return hitInfo.gameObject;
}

void PhysicsSystem::Clear()
{
	colliders.clear();
	pendingAddColliders.clear();
	pendingRemoveColliders.clear();
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

void PhysicsSystem::ApplyPendingColliderChanges()
{
	if (pendingRemoveColliders.empty() == false)
	{
		std::sort(pendingRemoveColliders.begin(), pendingRemoveColliders.end());
		pendingRemoveColliders.erase(std::unique(pendingRemoveColliders.begin(), pendingRemoveColliders.end()), pendingRemoveColliders.end());
		for (Collider* collider : pendingRemoveColliders)
		{
			if (collider == nullptr)
			{
				continue;
			}

			std::vector<Collider*>::iterator it{ std::find(colliders.begin(), colliders.end(), collider) };
			if (it != colliders.end())
			{
				const bool wasStatic{ (*it)->IsStatic() };
				colliders.erase(it);
				RemoveColliderFromCollisionHistory(collider);
				if (wasStatic)
				{
					RegisterStaticObjectsToGrid();
				}
			}
		}
		pendingRemoveColliders.clear();
	}

	if (pendingAddColliders.empty() == false)
	{
		std::sort(pendingAddColliders.begin(), pendingAddColliders.end());
		pendingAddColliders.erase(std::unique(pendingAddColliders.begin(), pendingAddColliders.end()), pendingAddColliders.end());
		for (Collider* collider : pendingAddColliders)
		{
			if (collider == nullptr)
			{
				continue;
			}
			if (std::find(colliders.begin(), colliders.end(), collider) == colliders.end())
			{
				colliders.emplace_back(collider);
				if (collider->IsStatic())
				{
					RegisterStaticObjectsToGrid();
				}
			}
		}
		pendingAddColliders.clear();
	}
}

void PhysicsSystem::RemoveColliderFromCollisionHistory(Collider* collider_)
{
	if (collider_ == nullptr)
	{
		return;
	}

	std::erase_if(previousCollisions, [collider_](const ColliderPair& pair)
	{
		return pair.c1 == collider_ || pair.c2 == collider_;
	});
}

void PhysicsSystem::ProcessPhysics(float fixedDeltaTime_)
{
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
		if (col == nullptr || col->GetOwner() == nullptr || col->GetOwner()->IsDestroyed() || col->GetOwner()->IsActive() == false)
		{
			continue;
		}

		if (col->IsStatic() == false)
		{
			dynamicColliders.emplace_back(col);
			
			const DirectX::BoundingBox aabb{ col->GetBoundingVolume() };
			
			const int minX{ static_cast<int>((aabb.Center.x - aabb.Extents.x - gridOrigin.x) / cellSize) };
			const int minZ{ static_cast<int>((aabb.Center.z - aabb.Extents.z - gridOrigin.z) / cellSize) };
			const int maxX{ static_cast<int>((aabb.Center.x + aabb.Extents.x - gridOrigin.x) / cellSize) };
			const int maxZ{ static_cast<int>((aabb.Center.z + aabb.Extents.z - gridOrigin.z) / cellSize) };

			for (int z{ minZ }; z <= maxZ; ++z)
			{
				for (int x{ minX }; x <= maxX; ++x)
				{
					if (x >= 0 && x < gridWidth && z >= 0 && z < gridHeight)
					{
						grid[z][x].dynamicColliders.emplace_back(col);
					}
				}
			}
		}
	}

	std::set<ColliderPair> currentCollisions;
	for (Collider* col1 : dynamicColliders)
	{
		if (col1 == nullptr || col1->GetOwner() == nullptr)
		{
			continue;
		}
		const DirectX::BoundingBox aabb{ col1->GetBoundingVolume() };
		
		const int minX{ static_cast<int>((aabb.Center.x - aabb.Extents.x - gridOrigin.x) / cellSize) };
		const int minZ{ static_cast<int>((aabb.Center.z - aabb.Extents.z - gridOrigin.z) / cellSize) };
		const int maxX{ static_cast<int>((aabb.Center.x + aabb.Extents.x - gridOrigin.x) / cellSize) };
		const int maxZ{ static_cast<int>((aabb.Center.z + aabb.Extents.z - gridOrigin.z) / cellSize) };

		for (int z{ minZ }; z <= maxZ; ++z)
		{
			for (int x{ minX }; x <= maxX; ++x)
			{
				if (x >= 0 && x < gridWidth && z >= 0 && z < gridHeight)
				{
					for (Collider* col2 : grid[z][x].dynamicColliders)
					{
						if (col2 == nullptr || col2->GetOwner() == nullptr)
						{
							continue;
						}
						if (reinterpret_cast<uintptr_t>(col1) <= reinterpret_cast<uintptr_t>(col2))
						{
							continue;
						}
						if (col1->IsIntersects(col2))
						{
							currentCollisions.insert({col1, col2});
						}
					}
					for (Collider* col2 : grid[z][x].staticColliders)
					{
						if (col2 == nullptr || col2->GetOwner() == nullptr)
						{
							continue;
						}
						if (col1->IsIntersects(col2))
						{
							const uintptr_t ptr1{ reinterpret_cast<uintptr_t>(col1) };
							const uintptr_t ptr2{ reinterpret_cast<uintptr_t>(col2) };
							currentCollisions.insert(ptr1 < ptr2 ? ColliderPair{col1, col2} : ColliderPair{col2, col1});
						}
					}
				}
			}
		}
	}
	
	for (const ColliderPair& pair : currentCollisions)
	{
		if (pair.c1 == nullptr || pair.c2 == nullptr || pair.c1->GetOwner() == nullptr || pair.c2->GetOwner() == nullptr)
		{
			continue;
		}

		GameObject* owner1{ pair.c1->GetOwner() };
		GameObject* owner2{ pair.c2->GetOwner() };
		if (owner1 == nullptr || owner2 == nullptr || owner1->IsDestroyed() || owner2->IsDestroyed())
		{
			continue;
		}

		if (previousCollisions.find(pair) != previousCollisions.end())
		{
			owner1->NotifyCollisionStay(pair.c2);
			if (pair.c2->GetOwner() != nullptr && pair.c2->GetOwner()->IsDestroyed() == false)
			{
				pair.c2->GetOwner()->NotifyCollisionStay(pair.c1);
			}
		}
		else
		{
			owner1->NotifyCollisionEnter(pair.c2);
			if (pair.c2->GetOwner() != nullptr && pair.c2->GetOwner()->IsDestroyed() == false)
			{
				pair.c2->GetOwner()->NotifyCollisionEnter(pair.c1);
			}
		}
	}

	for (const ColliderPair& pair : previousCollisions)
	{
		if (pair.c1 == nullptr || pair.c2 == nullptr || pair.c1->GetOwner() == nullptr || pair.c2->GetOwner() == nullptr)
		{
			continue;
		}

		if (currentCollisions.find(pair) == currentCollisions.end())
		{
			pair.c1->GetOwner()->NotifyCollisionExit(pair.c2);
			pair.c2->GetOwner()->NotifyCollisionExit(pair.c1);
		}
	}

	previousCollisions = std::move(currentCollisions);
}

std::vector<Collider*> PhysicsSystem::OverlapBox(const Vector3D& center_, const Vector3D& halfExtents_) const
{
	std::vector<Collider*> result;

	DirectX::BoundingBox queryBox{};
	queryBox.Center = center_;
	queryBox.Extents = halfExtents_;

	for (Collider* col : colliders)
	{
		if (col == nullptr || col->GetOwner() == nullptr || col->GetOwner()->IsActive() == false)
		{
			continue;
		}

		if (queryBox.Intersects(col->GetBoundingVolume()))
		{
			result.emplace_back(col);
		}
	}

	return result;
}

void PhysicsSystem::CreateGrid()
{
	grid.assign(gridHeight, std::vector<Cell>(gridWidth));
	isGridInitialized = true;
}

void PhysicsSystem::RegisterStaticObjectsToGrid()
{
	if (isGridInitialized == false)
	{
		return;
	}

	for (std::vector<Cell>& row : grid)
	{
		for (Cell& cell : row)
		{
			cell.staticColliders.clear();
		}
	}

	for (Collider* col : colliders)
	{
		if (col == nullptr || col->GetOwner() == nullptr || col->GetOwner()->IsActive() == false)
		{
			continue;
		}

		if (col->IsStatic())
		{
			const DirectX::BoundingBox aabb{ col->GetBoundingVolume() };

			const int minX{ static_cast<int>((aabb.Center.x - aabb.Extents.x - gridOrigin.x) / cellSize) };
			const int minZ{ static_cast<int>((aabb.Center.z - aabb.Extents.z - gridOrigin.z) / cellSize) };
			const int maxX{ static_cast<int>((aabb.Center.x + aabb.Extents.x - gridOrigin.x) / cellSize) };
			const int maxZ{ static_cast<int>((aabb.Center.z + aabb.Extents.z - gridOrigin.z) / cellSize) };

			for (int z{ minZ }; z <= maxZ; ++z)
			{
				for (int x{ minX }; x <= maxX; ++x)
				{
					if (x >= 0 && x < gridWidth && z >= 0 && z < gridHeight)
					{
						grid[z][x].staticColliders.emplace_back(col);
					}
				}
			}
		}
	}
}

std::vector<Collider*> PhysicsSystem::GetNearbyStaticColliders(Collider* collider_) const
{
	std::vector<Collider*> result;
	if (collider_ == nullptr || collider_->GetOwner() == nullptr)
	{
		return result;
	}

	const DirectX::BoundingBox aabb{ collider_->GetBoundingVolume() };

	const int minX{ static_cast<int>((aabb.Center.x - aabb.Extents.x - gridOrigin.x) / cellSize) };
	const int minZ{ static_cast<int>((aabb.Center.z - aabb.Extents.z - gridOrigin.z) / cellSize) };
	const int maxX{ static_cast<int>((aabb.Center.x + aabb.Extents.x - gridOrigin.x) / cellSize) };
	const int maxZ{ static_cast<int>((aabb.Center.z + aabb.Extents.z - gridOrigin.z) / cellSize) };

	const bool outOfGrid{
		maxX < 0 || maxZ < 0 || minX >= gridWidth || minZ >= gridHeight
	};

	if (!outOfGrid)
	{
		for (int z{ minZ }; z <= maxZ; ++z)
		{
			for (int x{ minX }; x <= maxX; ++x)
			{
				if (x >= 0 && x < gridWidth && z >= 0 && z < gridHeight)
				{
					for (Collider* staticCol : grid[z][x].staticColliders)
					{
						if (staticCol == nullptr || staticCol == collider_ || staticCol->GetOwner() == nullptr)
						{
							continue;
						}
						result.emplace_back(staticCol);
					}
				}
			}
		}
	}

	// Fallback: If target is outside grid or grid query yields nothing, scan all static colliders.
	if (outOfGrid || result.empty())
	{
		for (Collider* staticCol : colliders)
		{
			if (staticCol == nullptr || staticCol == collider_ || staticCol->GetOwner() == nullptr)
			{
				continue;
			}
			if (!staticCol->IsStatic())
			{
				continue;
			}
			result.emplace_back(staticCol);
		}
	}

	std::sort(result.begin(), result.end());
	result.erase(std::unique(result.begin(), result.end()), result.end());

	return result;
}
