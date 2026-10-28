#include "Precompiled.h"

#include "PhysicsSystem.h"

#include "Collider.h"
#include "GameObject.h"
#include "Transform.h"

void PhysicsSystem::Reset()
{
	grid.assign(gridHeight, std::vector<Cell>(gridWidth));
	isGridInitialized = true;
}

void PhysicsSystem::Update()
{
	ApplyPendingColliderChanges();

	if (!isGridInitialized
		|| static_cast<int>(grid.size()) != gridHeight
		|| (grid.empty() == false && static_cast<int>(grid.front().size()) != gridWidth))
	{
		Reset();
	}

	ProcessPhysics();
	
	// 물리 연산 중 발생한 파괴(Destroy) 요청을 즉시 처리하여, 
	// 이후 Scene::FixedUpdate에서 메모리가 해제되기 전에 활성 목록에서 깔끔하게 제거합니다.
	ApplyPendingColliderChanges();
}

void PhysicsSystem::AddCollider(Collider* collider_)
{
	assert(collider_ != nullptr);
	pendingAddColliders.emplace_back(collider_);
}

void PhysicsSystem::RemoveCollider(Collider* collider_)
{
	assert(collider_ != nullptr);
	pendingRemoveColliders.emplace_back(collider_);
}

bool PhysicsSystem::CheckCollision(Collider* collider_) const
{
	return IsCollidingWithStatic(collider_);
}

bool PhysicsSystem::IsCollidingWithStatic(Collider* collider_) const
{
	if (collider_ == nullptr)
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
					if (staticCol == nullptr || staticCol == collider_)
					{
						continue;
					}

					GameObject* const staticOwner{ staticCol->GetOwner() };
					if (staticOwner == nullptr || staticOwner->IsDestroyed())
					{
						continue;
					}

					const std::wstring& name{ staticOwner->GetName() };
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
	bool needsGridUpdate{ false };

	if (pendingAddColliders.empty() == false)
	{
		for (Collider* collider : pendingAddColliders)
		{
			if (collider == nullptr)
			{
				continue;
			}

			if (std::find(colliders.begin(), colliders.end(), collider) == colliders.end())
			{
				if (collider->IsStatic())
				{
					needsGridUpdate = true;
				}
				colliders.emplace_back(collider);
			}
		}
		pendingAddColliders.clear();
	}

	if (pendingRemoveColliders.empty() == false)
	{
		for (Collider* collider : pendingRemoveColliders)
		{
			if (collider == nullptr)
			{
				continue;
			}

			auto it{ std::find(colliders.begin(), colliders.end(), collider) };
			if (it != colliders.end())
			{
				if (collider->IsStatic())
				{
					needsGridUpdate = true;
				}
				colliders.erase(it);
				RemoveColliderFromCollisionHistory(collider);
			}
		}
		pendingRemoveColliders.clear();
	}

	if (needsGridUpdate)
	{
		RegisterStaticObjectsToGrid();
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

void PhysicsSystem::ProcessPhysics()
{
	for (std::vector<Cell>& row : grid)
	{
		for (Cell& cell : row)
		{
			cell.dynamicColliders.clear();
		}
	}
	
	std::vector<Collider*> dynamicColliders;
	std::vector<Collider*> outOfGridDynamicColliders;

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

			bool addedToGrid{ false };
			for (int z{ minZ }; z <= maxZ; ++z)
			{
				for (int x{ minX }; x <= maxX; ++x)
				{
					if (x >= 0 && x < gridWidth && z >= 0 && z < gridHeight)
					{
						grid[z][x].dynamicColliders.emplace_back(col);
						addedToGrid = true;
					}
				}
			}

			if (!addedToGrid)
			{
				outOfGridDynamicColliders.emplace_back(col);
			}
		}
	}

	std::set<ColliderPair> currentCollisions;
	for (Collider* col1 : dynamicColliders)
	{
		if (col1 == nullptr)
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
						if (col2 == nullptr)
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
						if (col2 == nullptr)
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

		// 그리드를 벗어난 객체들과의 충돌도 반드시 검사해야 합니다.
		for (Collider* col2 : outOfGridDynamicColliders)
		{
			if (col2 == nullptr || col1 == col2)
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
	}
	
	for (const ColliderPair& pair : currentCollisions)
	{
		if (pair.c1 == nullptr || pair.c2 == nullptr)
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
			GameObject* const otherOwner{ pair.c2->GetOwner() };
			if (otherOwner != nullptr && otherOwner->IsDestroyed() == false)
			{
				otherOwner->NotifyCollisionStay(pair.c1);
			}
		}
		else
		{
			owner1->NotifyCollisionEnter(pair.c2);
			GameObject* const otherOwner{ pair.c2->GetOwner() };
			if (otherOwner != nullptr && otherOwner->IsDestroyed() == false)
			{
				otherOwner->NotifyCollisionEnter(pair.c1);
			}
		}
	}

	for (const ColliderPair& pair : previousCollisions)
	{
		if (pair.c1 == nullptr || pair.c2 == nullptr)
		{
			continue;
		}

		if (currentCollisions.find(pair) == currentCollisions.end())
		{
			GameObject* const owner1{ pair.c1->GetOwner() };
			GameObject* const owner2{ pair.c2->GetOwner() };
			if (owner1 == nullptr || owner2 == nullptr || owner1->IsDestroyed() || owner2->IsDestroyed())
			{
				continue;
			}

			owner1->NotifyCollisionExit(pair.c2);
			owner2->NotifyCollisionExit(pair.c1);
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
		if (col == nullptr)
		{
			continue;
		}

		GameObject* const owner{ col->GetOwner() };
		if (owner == nullptr || owner->IsDestroyed() || owner->IsActive() == false)
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
		if (col == nullptr)
		{
			continue;
		}

		GameObject* const owner{ col->GetOwner() };
		if (owner == nullptr || owner->IsDestroyed() || owner->IsActive() == false)
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
	if (collider_ == nullptr)
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
						if (staticCol == nullptr || staticCol == collider_)
						{
							continue;
						}

						GameObject* const staticOwner{ staticCol->GetOwner() };
						if (staticOwner == nullptr || staticOwner->IsDestroyed())
						{
							continue;
						}
						result.emplace_back(staticCol);
					}
				}
			}
		}
	}

	if (outOfGrid || result.empty())
	{
		for (Collider* staticCol : colliders)
		{
			if (staticCol == nullptr || staticCol == collider_)
			{
				continue;
			}

			GameObject* const staticOwner{ staticCol->GetOwner() };
			if (staticOwner == nullptr || staticOwner->IsDestroyed())
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
