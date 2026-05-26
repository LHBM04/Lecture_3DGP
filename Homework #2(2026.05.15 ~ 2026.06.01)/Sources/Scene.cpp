#include "Precompiled.h"
#include "Scene.h"

#include "Camera.h"
#include "Collider.h"
#include "CubeCollider.h"
#include "GameObject.h"
#include "InputSystem.h"
#include "Light.h"
#include "Logger.h"
#include "MeshRenderer.h"
#include "InputContext.h"
#include "RenderContext.h"
#include "ResourceSystem.h"
#include "SceneContext.h"
#include "Transform.h"

void Scene::AddCamera(Camera* camera_)
{
	if (nullptr == camera_ || cameras.end() != std::find(cameras.begin(), cameras.end(), camera_))
	{
		return;
	}

	cameras.push_back(camera_);
}

void Scene::RemoveCamera(Camera* camera_)
{
	std::erase(cameras, camera_);
}

void Scene::AddLight(Light* light_)
{
	if (nullptr == light_ || lights.end() != std::find(lights.begin(), lights.end(), light_))
	{
		return;
	}

	lights.push_back(light_);
}

void Scene::RemoveLight(Light* light_)
{
	std::erase(lights, light_);
}

GameObject& Scene::CreateGameObject(const std::string& name_)
{
	std::unique_ptr<GameObject> object{ std::make_unique<GameObject>() };
	object->currentScene = this;
	object->SetName(name_);

	GameObject& reference{ *object };
	gameObjects.emplace_back(std::move(object));
	return reference;
}

void Scene::Load(SceneContext& context_)
{
	if (isLoaded)
	{
		return;
	}

	context = &context_;
	isLoaded = true;
	OnLoad();
}

void Scene::Update(const TimeContext& context_)
{
	if (!isLoaded)
	{
		return;
	}

	OnUpdate(context_);

	for (const std::unique_ptr<GameObject>& object : gameObjects)
	{
		if (object->IsActive())
		{
			object->Update(context_);
		}
	}

	CollectSceneTransitionRequests();
	RemoveDestroyedGameObjects();

	PickAtMouse();
}

void Scene::FixedUpdate(const TimeContext& context_)
{
	if (!isLoaded)
	{
		return;
	}

	OnFixedUpdate(context_);

	for (const std::unique_ptr<GameObject>& object : gameObjects)
	{
		if (object->IsActive())
		{
			object->FixedUpdate(context_);
		}
	}

	RemoveDestroyedGameObjects();
	DispatchCollisionEvents();
}

void Scene::Render(RenderContext& context_)
{
	InputSystem* inputSystem{ nullptr != context ? context->GetInputSystem() : nullptr };

	for (Camera* camera : cameras)
	{
		context_.SetCamera(camera);
		context_.Clear();
		if (!lights.empty())
		{
			context_.SetLight(lights.front());
		}

		if (nullptr == inputSystem)
		{
			continue;
		}
		const auto [screenWidth, screenHeight]{ inputSystem->GetScreenSize() };
		const int renderWidth{ std::max(1, screenWidth) };
		const int renderHeight{ std::max(1, screenHeight) };
		const float aspectRatio{ static_cast<float>(renderWidth) / static_cast<float>(renderHeight) };
		DirectX::BoundingFrustum cameraFrustum{};
		camera->GetWorldFrustum(aspectRatio, cameraFrustum);

		for (const std::unique_ptr<GameObject>& gameObject : gameObjects)
		{
			if (!gameObject->IsActive())
			{
				continue;
			}

			if (const Collider* collider{ gameObject->GetComponentInDerived<Collider>() })
			{
				if (!collider->Intersects(cameraFrustum))
				{
					continue;
				}
			}

			gameObject->Render(context_);
		}
	}

	context_.ResetViewport();
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects)
	{
		if (gameObject->IsActive())
		{
			gameObject->RenderUI(context_);
		}
	}
}

void Scene::DispatchInput(const InputContext& context_)
{
	if (!isLoaded)
	{
		return;
	}

	for (const std::unique_ptr<GameObject>& object : gameObjects)
	{
		if (object->IsActive())
		{
			object->DispatchInput(context_);
		}
	}
}

void Scene::Unload()
{
	if (!isLoaded)
	{
		return;
	}

	OnUnload();
	activeCollisionPairs.clear();
	context = nullptr;
	isLoaded = false;
}

SceneContext* Scene::GetSceneContext() noexcept
{
	return context;
}

const SceneContext* Scene::GetSceneContext() const noexcept
{
	return context;
}

void Scene::CollectSceneTransitionRequests()
{
	if (nullptr == context)
	{
		return;
	}

	for (const std::unique_ptr<GameObject>& gameObject : gameObjects)
	{
		if (nullptr == gameObject || !gameObject->IsActive())
		{
			continue;
		}

		for (SceneTransitionRequest* request : gameObject->GetSceneTransitionRequests())
		{
			if (nullptr == request || !request->HasSceneTransitionRequest())
			{
				continue;
			}

			context->RequestSceneChange(request->GetTargetSceneName());
			request->ClearSceneTransitionRequest();
			return;
		}
	}
}

void Scene::PickAtMouse()
{
	InputSystem* inputSystem{ nullptr != context ? context->GetInputSystem() : nullptr };
	if (nullptr == inputSystem || cameras.empty() || !inputSystem->IsButtonPressed(ButtonCode::Left))
	{
		return;
	}

	Camera* camera{ cameras.front() };
	if (nullptr == camera || nullptr == camera->GetOwner())
	{
		return;
	}

	Transform* cameraTransform{ camera->GetOwner()->GetComponent<Transform>() };
	if (nullptr == cameraTransform)
	{
		return;
	}

	const auto [inputWidth, inputHeight]{ inputSystem->GetScreenSize() };
	const int screenWidth{ std::max(1, inputWidth) };
	const int screenHeight{ std::max(1, inputHeight) };
	const auto [mouseX, mouseY]{ inputSystem->GetMousePosition() };

	const float px{ (static_cast<float>(mouseX) / static_cast<float>(screenWidth)) * 2.0f - 1.0f };
	const float py{ 1.0f - (static_cast<float>(mouseY) / static_cast<float>(screenHeight)) * 2.0f };

	const float aspect{ static_cast<float>(screenWidth) / static_cast<float>(screenHeight) };
	const Matrix4x4 view{ camera->GetViewMatrix() };
	const Matrix4x4 projection{ camera->GetProjectionMatrix(aspect) };
	const Matrix4x4 inverseViewProjection{ (view * projection).GetInverse() };

	const DirectX::XMVECTOR nearNdc{ DirectX::XMVectorSet(px, py, 0.0f, 1.0f) };
	const DirectX::XMVECTOR farNdc{ DirectX::XMVectorSet(px, py, 1.0f, 1.0f) };
	const DirectX::XMMATRIX invVP{ Matrix4x4::Load(inverseViewProjection) };

	DirectX::XMVECTOR nearWorld{ DirectX::XMVector4Transform(nearNdc, invVP) };
	DirectX::XMVECTOR farWorld{ DirectX::XMVector4Transform(farNdc, invVP) };
	nearWorld = DirectX::XMVectorScale(nearWorld, 1.0f / DirectX::XMVectorGetW(nearWorld));
	farWorld = DirectX::XMVectorScale(farWorld, 1.0f / DirectX::XMVectorGetW(farWorld));

	const DirectX::XMVECTOR rayOrigin{ nearWorld };
	const DirectX::XMVECTOR rayDirection{
		DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(farWorld, nearWorld))
	};

	GameObject* pickedObject{ nullptr };
	float nearestDistance{ std::numeric_limits<float>::max() };

	for (const std::unique_ptr<GameObject>& gameObject : gameObjects)
	{
		if (!gameObject->IsActive())
		{
			continue;
		}

		if (nullptr == gameObject->GetComponent<MeshRenderer>())
		{
			continue;
		}

		Transform* transform{ gameObject->GetComponent<Transform>() };
		if (nullptr == transform)
		{
			continue;
		}

		const Vector3D worldPosition{ transform->GetWorldPosition() };
		const Vector3D worldScale{ transform->GetWorldScale() };
		const float radius{
			std::max(0.25f, std::max(worldScale.x, std::max(worldScale.y, worldScale.z)) * 0.6f)
		};

		DirectX::BoundingSphere sphere{
			DirectX::XMFLOAT3(worldPosition.x, worldPosition.y, worldPosition.z),
			radius
		};

		float distance{ 0.0f };
		if (!sphere.Intersects(rayOrigin, rayDirection, distance))
		{
			continue;
		}

		if (distance < nearestDistance)
		{
			nearestDistance = distance;
			pickedObject = gameObject.get();
		}
	}

	if (nullptr != pickedObject)
	{
		Logger::Info("Picked: %s", pickedObject->GetName().c_str());
	}
	else
	{
		Logger::Info("Picked: none");
	}
}

void Scene::DispatchCollisionEvents()
{
	constexpr float GridCellSize{ 4.0f };

	const bool hasCollisionListeners
	{
		std::ranges::any_of(gameObjects, [](const std::unique_ptr<GameObject>& object)
			{
				return object->IsActive() && object->HasCollisionListeners();
			})
	};

	if (!hasCollisionListeners)
	{
		activeCollisionPairs.clear();
		return;
	}

	struct ColliderEntry
	{
		GameObject* object{ nullptr };
		Collider* collider{ nullptr };
		const CubeCollider* cubeCollider{ nullptr };
		DirectX::BoundingOrientedBox box{};
		DirectX::BoundingSphere broadSphere{};
	};

	struct CellCoord
	{
		int x{ 0 };
		int y{ 0 };
		int z{ 0 };

		bool operator==(const CellCoord& other_) const noexcept
		{
			return x == other_.x && y == other_.y && z == other_.z;
		}
	};

	struct CellCoordHash
	{
		std::size_t operator()(const CellCoord& coord_) const noexcept
		{
			const std::size_t hx{ static_cast<std::size_t>(coord_.x) * 73856093u };
			const std::size_t hy{ static_cast<std::size_t>(coord_.y) * 19349663u };
			const std::size_t hz{ static_cast<std::size_t>(coord_.z) * 83492791u };
			return hx ^ hy ^ hz;
		}
	};

	std::vector<ColliderEntry> colliders;
	colliders.reserve(gameObjects.size());

	for (const std::unique_ptr<GameObject>& gameObject : gameObjects)
	{
		GameObject* object{ gameObject.get() };
		if (nullptr == object || !object->IsActive())
		{
			continue;
		}

		Collider* collider{ object->GetComponentInDerived<Collider>() };
		if (nullptr != collider)
		{
			ColliderEntry entry{};
			entry.object = object;
			entry.collider = collider;
			entry.cubeCollider = dynamic_cast<const CubeCollider*>(collider);
			if (nullptr != entry.cubeCollider)
			{
				entry.box = entry.cubeCollider->GetWorldOrientedBox();
				const DirectX::XMVECTOR extents{
					DirectX::XMVectorSet(entry.box.Extents.x, entry.box.Extents.y, entry.box.Extents.z, 0.0f)
				};
				const float radius{ DirectX::XMVectorGetX(DirectX::XMVector3Length(extents)) };
				entry.broadSphere.Center = entry.box.Center;
				entry.broadSphere.Radius = radius;
			}

			colliders.push_back(entry);
		}
	}
	std::unordered_set<CollisionPair, CollisionPairHash> currentPairs;
	currentPairs.reserve(activeCollisionPairs.size());
	std::unordered_set<CollisionPair, CollisionPairHash> testedPairs;
	testedPairs.reserve(colliders.size() * 2u);
	std::unordered_map<CellCoord, std::vector<std::size_t>, CellCoordHash> grid;
	grid.reserve(colliders.size() * 2u);
	std::vector<std::size_t> nonCubeIndices;
	nonCubeIndices.reserve(colliders.size());

	const auto buildCollisionPair = [](const GameObject* a_, const GameObject* b_) noexcept
		{
			return a_ < b_ ? CollisionPair{ a_, b_ } : CollisionPair{ b_, a_ };
		};

	const auto toCell = [](float value_) noexcept
		{
			return static_cast<int>(std::floor(value_ / GridCellSize));
		};

	for (std::size_t i{ 0 }; i < colliders.size(); ++i)
	{
		const ColliderEntry& entry{ colliders[i] };
		if (nullptr == entry.cubeCollider)
		{
			nonCubeIndices.push_back(i);
			continue;
		}

		const DirectX::XMFLOAT3& center{ entry.broadSphere.Center };
		const float radius{ std::max(0.0f, entry.broadSphere.Radius) };

		const int minX{ toCell(center.x - radius) };
		const int minY{ toCell(center.y - radius) };
		const int minZ{ toCell(center.z - radius) };
		const int maxX{ toCell(center.x + radius) };
		const int maxY{ toCell(center.y + radius) };
		const int maxZ{ toCell(center.z + radius) };

		for (int z{ minZ }; z <= maxZ; ++z)
		{
			for (int y{ minY }; y <= maxY; ++y)
			{
				for (int x{ minX }; x <= maxX; ++x)
				{
					grid[CellCoord{ x, y, z }].push_back(i);
				}
			}
		}
	}

	const auto testPair = [&](std::size_t lhsIndex_, std::size_t rhsIndex_)
	{
		ColliderEntry& lhs{ colliders[lhsIndex_] };
		ColliderEntry& rhs{ colliders[rhsIndex_] };
		CollisionPair pair{ buildCollisionPair(lhs.object, rhs.object) };
		if (testedPairs.contains(pair))
		{
			return;
		}
		testedPairs.insert(pair);

		const CubeCollider* lhsCube{ lhs.cubeCollider };
		const CubeCollider* rhsCube{ rhs.cubeCollider };
		if (nullptr != lhsCube && nullptr != rhsCube)
		{
			if (!lhs.broadSphere.Intersects(rhs.broadSphere))
			{
				return;
			}

			if (!lhs.box.Intersects(rhs.box))
			{
				return;
			}
		}
		else if (!lhs.collider->Intersects(*rhs.collider))
		{
			return;
		}

		if (!lhs.object->HasCollisionListeners() && !rhs.object->HasCollisionListeners())
		{
			return;
		}

		currentPairs.insert(pair);

		const bool isNewPair{ !activeCollisionPairs.contains(pair) };
		if (isNewPair)
		{
			lhs.object->NotifyCollisionEnter(*rhs.object);
			rhs.object->NotifyCollisionEnter(*lhs.object);
		}
		else
		{
			lhs.object->NotifyCollisionStay(*rhs.object);
			rhs.object->NotifyCollisionStay(*lhs.object);
		}
	};

	for (const auto& [cell, indices] : grid)
	{
		(void)cell;
		for (std::size_t i{ 0 }; i < indices.size(); ++i)
		{
			for (std::size_t j{ i + 1u }; j < indices.size(); ++j)
			{
				testPair(indices[i], indices[j]);
			}
		}
	}

	for (std::size_t i{ 0 }; i < nonCubeIndices.size(); ++i)
	{
		for (std::size_t j{ i + 1u }; j < nonCubeIndices.size(); ++j)
		{
			testPair(nonCubeIndices[i], nonCubeIndices[j]);
		}
	}

	for (const std::size_t nonCubeIndex : nonCubeIndices)
	{
		for (std::size_t cubeIndex{ 0 }; cubeIndex < colliders.size(); ++cubeIndex)
		{
			if (nullptr == colliders[cubeIndex].cubeCollider)
			{
				continue;
			}

			testPair(nonCubeIndex, cubeIndex);
		}
	}

	for (const CollisionPair& pair : activeCollisionPairs)
	{
		if (currentPairs.contains(pair))
		{
			continue;
		}

		GameObject* lhsObject{ const_cast<GameObject*>(pair.first) };
		GameObject* rhsObject{ const_cast<GameObject*>(pair.second) };
		lhsObject->NotifyCollisionExit(*rhsObject);
		rhsObject->NotifyCollisionExit(*lhsObject);
	}

	activeCollisionPairs = std::move(currentPairs);
}

void Scene::RemoveDestroyedGameObjects()
{
	const bool hasDestroyedObject
	{
		std::ranges::any_of(gameObjects, [](const std::unique_ptr<GameObject>& object)
			{
				return object->IsDestroyed();
			})
	};

	if (!hasDestroyedObject)
	{
		return;
	}

	std::vector<const GameObject*> destroyedObjects;
	destroyedObjects.reserve(gameObjects.size());

	for (const std::unique_ptr<GameObject>& object : gameObjects)
	{
		if (object->IsDestroyed())
		{
			destroyedObjects.push_back(object.get());
		}
	}

	const auto isDestroyed = [&destroyedObjects](const GameObject* object_) noexcept
		{
			return destroyedObjects.end() != std::find(destroyedObjects.begin(), destroyedObjects.end(), object_);
		};

	std::erase_if(activeCollisionPairs, [&isDestroyed](const CollisionPair& pair)
		{
			return isDestroyed(pair.first) || isDestroyed(pair.second);
		});

	std::erase_if(gameObjects, [](const auto& object)
		{
			return object->IsDestroyed();
		});
}
