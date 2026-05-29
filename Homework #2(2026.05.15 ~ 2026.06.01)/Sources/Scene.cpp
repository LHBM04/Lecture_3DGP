#include "Precompiled.h"
#include "Scene.h"

#include "Transform.h"

void Scene::Load()
{
	isLoaded = true;
}

void Scene::Unload()
{
	isLoaded = false;
}

void Scene::Update(float deltaTime_)
{
	assert(isLoaded);

	for (const auto& gameObject : gameObjects)
	{
		gameObject->Update(deltaTime_);
	}
}

void Scene::FixedUpdate(float fixedDeltaTime_)
{
	assert(isLoaded);

	for (const auto& gameObject : gameObjects)
	{
		gameObject->FixedUpdate(fixedDeltaTime_);
	}
}

void Scene::Render()
{
	assert(isLoaded);

	for (const auto& gameObject : gameObjects)
	{
		gameObject->Render();
	}
}

GameObject* Scene::CreateGameObject()
{
	std::unique_ptr<GameObject> newObject{ std::make_unique<GameObject>() };
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
