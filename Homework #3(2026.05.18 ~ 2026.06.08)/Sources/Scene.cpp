#include "Precompiled.h"
#include "Scene.h"

#include "Camera.h"
#include "Light.h"
#include "RenderSystem.h"
#include "Transform.h"

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

void Scene::Update()
{
	OnUpdate();

	for (const auto& gameObject : gameObjects)
	{
		gameObject->Update();
	}
}

void Scene::LateUpdate()
{
	OnLateUpdate();

	for (const auto& gameObject : gameObjects)
	{
		gameObject->LateUpdate();
	}
}

void Scene::FixedUpdate()
{
	OnFixedUpdate();

	for (auto gameObject{ gameObjects.begin() }; gameObject != gameObjects.end(); )
	{
		(*gameObject)->FixedUpdate();

		if ((*gameObject)->IsDestroyed())
		{
			gameObject->reset();
			gameObject = gameObjects.erase(gameObject);
		}
		else
		{
			++gameObject;
		}
	}
}

void Scene::Render(ID3D12GraphicsCommandList* commandList_)
{
	RenderSystem& renderSystem{ RenderSystem::GetInstance() };
	if (!cameras.empty())
	{
		renderSystem.SetCameraConstants(cameras.front()->GetCameraConstants());
	}
	else
	{
		renderSystem.SetCameraConstants(CameraConstants{});
	}

	if (!lights.empty())
	{
		renderSystem.SetLightConstants(lights.front()->GetLightConstants());
	}
	else
	{
		renderSystem.SetLightConstants(LightConstants{});
	}

	OnRender(commandList_);

	for (const auto& gameObject : gameObjects)
	{
		gameObject->Render(commandList_);
	}
}

GameObject* Scene::Instantiate()
{
	std::unique_ptr<GameObject> newObject{ std::make_unique<GameObject>() };
	newObject->currentScene = this;
	newObject->SetName(L"New GameObject");
	newObject->SetTag(L"Untagged");

	Transform* const transform{ newObject->AddComponent<Transform>() };
	if (transform != nullptr)
	{
		transform->SetLocalPosition(Vector3D::GetZero());
		transform->SetLocalRotation(Quaternion::GetIdentity());
		transform->SetLocalScale(Vector3D::GetOne());
		transform->SetParent(nullptr);
	}

	GameObject* const newObjectPtr{ newObject.get() };
	gameObjects.push_back(std::move(newObject));
	return newObjectPtr;
}

GameObject* Scene::Instantiate(const Vector3D& position_, const Quaternion& rotation_)
{
	GameObject* const newObject{ Instantiate() };
	if (newObject == nullptr)
	{
		return nullptr;
	}

	if (Transform* const transform{ newObject->GetTransform() }; transform != nullptr)
	{
		transform->SetWorldPosition(position_);
		transform->SetWorldRotation(rotation_);
	}
	return newObject;
}

GameObject* Scene::Instantiate(const Vector3D& position_)
{
	return Instantiate(position_, Quaternion::GetIdentity());
}

GameObject* Scene::Instantiate(const Quaternion& rotation_)
{
	return Instantiate(Vector3D::GetZero(), rotation_);
}

GameObject* Scene::Instantiate(Transform* parent_)
{
	GameObject* const newObject{ Instantiate() };
	if (newObject == nullptr)
	{
		return nullptr;
	}

	if (Transform* const transform{ newObject->GetTransform() }; transform != nullptr)
	{
		transform->SetParent(parent_);
	}
	return newObject;
}

GameObject* Scene::Instantiate(const Vector3D& position_, const Quaternion& rotation_, Transform* parent_)
{
	GameObject* const newObject{ Instantiate(position_, rotation_) };
	if (newObject == nullptr)
	{
		return nullptr;
	}

	if (Transform* const transform{ newObject->GetTransform() }; transform != nullptr)
	{
		transform->SetParent(parent_);
	}
	return newObject;
}

void Scene::AddCamera(Camera* camera_)
{
	if (camera_ != nullptr && !std::ranges::contains(cameras, camera_))
	{
		cameras.push_back(camera_);
	}
}

void Scene::RemoveCamera(Camera* camera_)
{
	std::erase(cameras, camera_);
}

void Scene::AddLight(Light* light_)
{
	if (light_ != nullptr && !std::ranges::contains(lights, light_))
	{
		lights.push_back(light_);
	}
}

void Scene::RemoveLight(Light* light_)
{
	std::erase(lights, light_);
}

const std::vector<Camera*>& Scene::GetCameras() const noexcept
{
	return cameras;
}

const std::vector<Light*>& Scene::GetLights() const noexcept
{
	return lights;
}

GameObject* Scene::FindObjectWithName(std::wstring_view name_)
{
	auto iter{ std::ranges::find_if(gameObjects, [name_](const auto& gameObject)
		{
			return gameObject->GetName() == name_;
		}) };
	if (iter != gameObjects.end())
	{
		return iter->get();
	}

	return nullptr;
}

auto Scene::FindObjectsWithName(std::wstring_view name_)
{
	return gameObjects
		| std::views::filter([name_](const auto& gameObject)
			{
				return gameObject->GetName() == name_;
			})
		| std::views::transform([](const auto& gameObject)
			{
				return gameObject.get();
			});
}

GameObject* Scene::FindObjectWithTag(std::wstring_view tag_)
{
	auto iter{ std::ranges::find_if(gameObjects, [tag_](const auto& gameObject)
		{
			return gameObject->GetTag() == tag_;
		}) };
	if (iter != gameObjects.end())
	{
		return iter->get();
	}
	return nullptr;
}

auto Scene::FindObjectsWithTag(std::wstring_view tag_)
{
	return gameObjects
		| std::views::filter([tag_](const auto& gameObject)
			{
				return gameObject->GetTag() == tag_;
			})
		| std::views::transform([](const auto& gameObject)
			{
				return gameObject.get();
			});
}
