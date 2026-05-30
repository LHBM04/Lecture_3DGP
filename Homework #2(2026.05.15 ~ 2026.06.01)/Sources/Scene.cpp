#include "Precompiled.h"
#include "Scene.h"

#include <algorithm>
#include <cassert>
#include <limits>

#include "Camera.h"
#include "CubeCollider.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "RenderSystem.h"
#include "SphereCollider.h"
#include "Transform.h"

Scene::Scene() = default;
Scene::~Scene() = default;

void Scene::Load()
{
	if (isLoaded)
	{
		return;
	}
	OnLoad();
	isLoaded = true;
}

void Scene::Unload()
{
	if (!isLoaded)
	{
		return;
	}
	OnUnload();
	isLoaded = false;
}

void Scene::Update(float deltaTime_)
{
	assert(isLoaded);

	for (const auto& go : gameObjects)
	{
		go->Update(deltaTime_);
	}
}

void Scene::FixedUpdate(float fixedDeltaTime_)
{
	assert(isLoaded);

	// 물리 시뮬레이션 고정 시간 간격으로 실행
	ProcessPhysics(fixedDeltaTime_);

	for (const auto& go : gameObjects)
	{
		go->FixedUpdate(fixedDeltaTime_);
	}
}

void Scene::Render()
{
	assert(isLoaded);

	for (const auto& camera : cameras)
	{
		if (!camera->GetOwner()->IsActive())
		{
			continue;
		}

		RenderSystem::GetInstance().SetCamera(camera);
		RenderSystem::GetInstance().SetLights(lights);

		for (const auto& gameObject : gameObjects)
		{
			if (!gameObject->IsActive())
			{
				continue;
			}

			bool inFrustum{ true };
			if (auto* sc = gameObject->GetComponent<SphereCollider>())
			{
				inFrustum = camera->IsInFrustum(sc);
			}
			else if (auto* cc = gameObject->GetComponent<CubeCollider>())
			{
				inFrustum = camera->IsInFrustum(cc);
			}

			if (!inFrustum)
			{
				continue;
			}

			gameObject->Render();
		}
	}
}

void Scene::ProcessPhysics(float fixedDeltaTime_)
{
	// DirectX의 Intersects 기능을 활용한 완전 정적 물리 처리
	for (size_t i{ 0 }; i < gameObjects.size(); ++i)
	{
		GameObject* go1{ gameObjects[i].get() };
		if (!go1->IsActive()) continue;

		auto* s1 = go1->GetComponent<SphereCollider>();
		auto* c1 = go1->GetComponent<CubeCollider>();
		if (!s1 && !c1) continue;

		for (size_t j{ i + 1 }; j < gameObjects.size(); ++j)
		{
			GameObject* go2{ gameObjects[j].get() };
			if (!go2->IsActive()) continue;

			auto* s2 = go2->GetComponent<SphereCollider>();
			auto* c2 = go2->GetComponent<CubeCollider>();
			if (!s2 && !c2) continue;

			bool isHit{ false };

			// 각 콜라이더의 Volume을 꺼내어 DirectX::Intersects에게 직접 맡김
			if (s1 && s2) isHit = s1->GetVolume().Intersects(s2->GetVolume());
			else if (s1 && c2) isHit = s1->GetVolume().Intersects(c2->GetVolume());
			else if (c1 && s2) isHit = c1->GetVolume().Intersects(s2->GetVolume());
			else if (c1 && c2) isHit = c1->GetVolume().Intersects(c2->GetVolume());

			if (isHit)
			{
				// 충돌 처리 로직
			}
		}
	}
}

GameObject* Scene::Pick(const Vector3D& rayOrigin_, const Vector3D& rayDir_, float* distance_)
{
	GameObject* closestObject{ nullptr };
	float minDistance{ std::numeric_limits<float>::max() };

	for (const auto& go : gameObjects)
	{
		if (!go->IsActive()) continue;

		float distance{ 0.0f };
		bool isHit{ false };

		// 레이 피킹도 개별 클래스 오버로딩 없이 Collider 베이스의 IsIntersects를 통해 통일됨
		if (auto* sc = go->GetComponent<SphereCollider>())
		{
			isHit = sc->IsIntersects(rayOrigin_, rayDir_, distance);
		}
		else if (auto* cc = go->GetComponent<CubeCollider>())
		{
			isHit = cc->IsIntersects(rayOrigin_, rayDir_, distance);
		}

		if (isHit && distance < minDistance)
		{
			minDistance = distance;
			closestObject = go.get();
		}
	}

	if (distance_)
	{
		*distance_ = minDistance;
	}
	return closestObject;
}

GameObject* Scene::CreateGameObject()
{
	std::unique_ptr<GameObject> newObject{ std::make_unique<GameObject>() };
	newObject->scene = this;
	newObject->SetName(L"New GameObject");
	newObject->SetTag(L"Untagged");

	Transform* transform{ newObject->AddComponent<Transform>() };
	transform->SetLocalPosition(Vector3D::GetZero());
	transform->SetLocalRotation(Quaternion::GetIdentity());
	transform->SetLocalScale(Vector3D::GetOne());
	transform->SetParent(nullptr);

	GameObject* newObjectPtr{ newObject.get() };
	gameObjects.push_back(std::move(newObject));

	return newObjectPtr;
}

void Scene::AddCamera(Camera* camera_)
{
	cameras.push_back(camera_);
}

void Scene::RemoveCamera(Camera* camera_)
{
	cameras.erase(std::remove(cameras.begin(), cameras.end(), camera_), cameras.end());
}

void Scene::AddLight(Light* light_)
{
	lights.push_back(light_);
}

void Scene::RemoveLight(Light* light_)
{
	lights.erase(std::remove(lights.begin(), lights.end(), light_), lights.end());
}

