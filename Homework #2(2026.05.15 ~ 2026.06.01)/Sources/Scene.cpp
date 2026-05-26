#include "Precompiled.h"
#include "Scene.h"

#include "Application.h"
#include "Camera.h"
#include "Collider.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Light.h"
#include "Logger.h"
#include "MeshRenderer.h"
#include "Renderer.h"
#include "ResourceManager.h"
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

void Scene::Load()
{
	if (isLoaded)
	{
		return;
	}

	isLoaded = true;
	OnLoad();
}

void Scene::Update()
{
	if (!isLoaded)
	{
		return;
	}

	OnUpdate();

	for (const std::unique_ptr<GameObject>& object : gameObjects)
	{
		if (object->IsActive())
		{
			object->Update();
		}
	}

	std::erase_if(gameObjects, [](const auto& object)
		{
			return object->IsDestroyed();
		});

	PickAtMouse();
}

void Scene::Render()
{
	Renderer& renderer{ Application::GetRenderer() };

	for (Camera* camera : cameras)
	{
		renderer.SetCamera(camera);
		renderer.Clear();
		if (!lights.empty())
		{
			renderer.SetLight(lights.front());
		}

		const int renderWidth{ std::max(1, renderer.GetWidth()) };
		const int renderHeight{ std::max(1, renderer.GetHeight()) };
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

			gameObject->Render();
		}

		renderer.Flush();
	}
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

void Scene::PickAtMouse()
{
	if (cameras.empty() || !InputManager::IsButtonPressed(ButtonCode::Left))
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

	Renderer& renderer{ Application::GetRenderer() };
	const int screenWidth{ std::max(1, renderer.GetWidth()) };
	const int screenHeight{ std::max(1, renderer.GetHeight()) };
	const auto [mouseX, mouseY]{ InputManager::GetMousePosition() };

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
