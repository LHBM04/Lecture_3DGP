#include "Precompiled.h"
#include "Scene.h"

#include "Camera.h"
#include "GameObject.h"
#include "Light.h"
#include "RenderContext.h"
#include "RenderService.h"
#include "Transform.h"
#include "UploadBuffer.h"

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

void Scene::Render(const RenderContext& context_)
{
	OnRender(context_);

	if (cameras.empty())
	{
		for (const auto& gameObject : gameObjects)
		{
			gameObject->Render(context_);
		}
		return;
	}

	ID3D12GraphicsCommandList& commandList{ context_.GetCommandList() };

	for (Camera* camera : cameras)
	{
		if (camera == nullptr)
		{
			continue;
		}

		switch (camera->GetClearMode())
		{
		case Camera::ClearType::SolidColor:
		{
			const ColorRGBA& clearColor{ camera->GetClearColor() };
			const FLOAT color[]{ clearColor.x, clearColor.y, clearColor.z, clearColor.w };
			commandList.ClearRenderTargetView(context_.GetRtvHandle(), color, 0, nullptr);
			commandList.ClearDepthStencilView(context_.GetDsvHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
			break;
		}
		case Camera::ClearType::DepthOnly:
		{
			commandList.ClearDepthStencilView(context_.GetDsvHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
			break;
		}
		case Camera::ClearType::Nothing:
			break;
		default:
			std::unreachable();
		}

		CameraConstants cameraConstants{};
		cameraConstants.viewMatrix = camera->GetViewMatrix();
		cameraConstants.projectionMatrix = camera->GetProjectionMatrix();
		cameraConstants.viewProjectionMatrix = camera->GetViewProjectionMatrix();

		const D3D12_GPU_VIRTUAL_ADDRESS cameraConstantsAddress{
			context_.GetUploadBuffer().Allocate(cameraConstants, sizeof(CameraConstants))
		};
		if (cameraConstantsAddress != 0)
		{
			commandList.SetGraphicsRootConstantBufferView(0, cameraConstantsAddress);
		}

		for (const auto& gameObject : gameObjects)
		{
			gameObject->Render(context_);
		}
	}
}

GameObject& Scene::Instantiate()
{
	std::unique_ptr<GameObject> newObject{ std::make_unique<GameObject>() };
	newObject->currentScene = this;
	newObject->SetName(L"New GameObject");
	newObject->SetTag(L"Untagged");

	Transform& transform{ newObject->AddComponent<Transform>() };
	transform.SetLocalPosition(Vector3D::GetZero());
	transform.SetLocalRotation(Quaternion::GetIdentity());
	transform.SetLocalScale(Vector3D::GetOne());
	transform.SetParent(nullptr);

	GameObject* const newObjectPtr{ newObject.get() };
	gameObjects.push_back(std::move(newObject));
	return *newObjectPtr;
}

GameObject& Scene::Instantiate(const Vector3D& position_, const Quaternion& rotation_)
{
	GameObject& newObject{ Instantiate() };
	Transform& transform{ newObject.GetTransform() };
	transform.SetWorldPosition(position_);
	transform.SetWorldRotation(rotation_);
	return newObject;
}

GameObject& Scene::Instantiate(const Vector3D& position_)
{
	return Instantiate(position_, Quaternion::GetIdentity());
}

GameObject& Scene::Instantiate(const Quaternion& rotation_)
{
	return Instantiate(Vector3D::GetZero(), rotation_);
}

GameObject& Scene::Instantiate(Transform* parent_)
{
	GameObject& newObject{ Instantiate() };
	newObject.GetTransform().SetParent(parent_);
	return newObject;
}

GameObject& Scene::Instantiate(const Vector3D& position_, const Quaternion& rotation_, Transform* parent_)
{
	GameObject& newObject{ Instantiate(position_, rotation_) };
	newObject.GetTransform().SetParent(parent_);
	return newObject;
}

void Scene::AddCamera(Camera& camera_)
{
	if (!std::ranges::contains(cameras, &camera_))
	{
		cameras.push_back(&camera_);
	}
}

void Scene::RemoveCamera(Camera& camera_)
{
	std::erase(cameras, &camera_);
}

void Scene::AddLight(Light& light_)
{
	if (!std::ranges::contains(lights, &light_))
	{
		lights.push_back(&light_);
	}
}

void Scene::RemoveLight(Light& light_)
{
	std::erase(lights, &light_);
}

void Scene::Destroy(GameObject& gameObject_)
{
	gameObject_.Destroy();
}

std::span<const std::unique_ptr<GameObject>> Scene::GetGameObjects() const noexcept
{
	return { gameObjects.data(), gameObjects.size() };
}

std::span<Camera* const> Scene::GetCameras() noexcept
{
	return { cameras.data(), cameras.size() };
}

std::span<Camera* const> Scene::GetCameras() const noexcept
{
	return { cameras.data(), cameras.size() };
}

std::span<Light* const> Scene::GetLights() noexcept
{
	return { lights.data(), lights.size() };
}

std::span<Light* const> Scene::GetLights() const noexcept
{
	return { lights.data(), lights.size() };
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
