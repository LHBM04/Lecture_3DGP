#include "Precompiled.h"
#include "Scene.h"

#include "Camera.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "RenderTarget.h"
#include "Transform.h"
#include "UIComponent.h"

namespace
{
	struct MeshBatchKey final
	{
		const Mesh* mesh{ nullptr };
		const Material* material{ nullptr };

		[[nodiscard]] bool operator==(const MeshBatchKey& other_) const noexcept
		{
			return mesh == other_.mesh && material == other_.material;
		}
	};

	struct MeshBatchKeyHash final
	{
		[[nodiscard]] std::size_t operator()(const MeshBatchKey& key_) const noexcept
		{
			const std::size_t meshHash{ std::hash<const Mesh*>{}(key_.mesh) };
			const std::size_t materialHash{ std::hash<const Material*>{}(key_.material) };
			return meshHash ^ (materialHash + 0x9e3779b97f4a7c15ULL + (meshHash << 6) + (meshHash >> 2));
		}
	};
}

void Scene::AddCamera(Camera* const camera_)
{
	if (nullptr == camera_ || std::find(cameras.begin(), cameras.end(), camera_) != cameras.end())
	{
		return;
	}

	cameras.push_back(camera_);
	if (nullptr == mainCamera)
	{
		mainCamera = camera_;
	}
}

void Scene::RemoveCamera(Camera* const camera_)
{
	cameras.erase(std::remove(cameras.begin(), cameras.end(), camera_), cameras.end());
	if (mainCamera == camera_)
	{
		mainCamera = cameras.empty() ? nullptr : cameras.front();
	}
}

void Scene::SetMainCamera(Camera* camera_)
{
	if (nullptr != camera_ && std::find(cameras.begin(), cameras.end(), camera_) == cameras.end())
	{
		AddCamera(camera_);
	}

	mainCamera = camera_;
}

Camera* Scene::GetMainCamera() noexcept
{
	return mainCamera;
}

const Camera* Scene::GetMainCamera() const noexcept
{
	return mainCamera;
}

const std::vector<Camera*>& Scene::GetCameras() const noexcept
{
	return cameras;
}

GameObject* Scene::AddGameObject()
{
	std::unique_ptr<GameObject> newObject = std::make_unique<GameObject>();
	newObject->SetCurrentScene(this);
	newObject->SetName("New Object");
	newObject->SetTag("Untagged");

	objects.push_back(std::move(newObject));
	return objects.back().get();
}

GameObject* Scene::AddUIGameObject()
{
	std::unique_ptr<GameObject> newObject = std::make_unique<GameObject>(true);
	newObject->SetCurrentScene(this);
	newObject->SetName("New UI Object");
	newObject->SetTag("UI");

	objects.push_back(std::move(newObject));
	return objects.back().get();
}

void Scene::RemoveGameObject(GameObject* const gameObject_)
{
	objects.erase(std::remove_if(objects.begin(), objects.end(),
		[gameObject_](const std::unique_ptr<GameObject>& object)
		{
			return object.get() == gameObject_;
		}), objects.end());
}

void Scene::AddUIComponent(UIComponent* const uiComponent_)
{
	if (nullptr == uiComponent_ || std::find(uiComponents.begin(), uiComponents.end(), uiComponent_) != uiComponents.end())
	{
		return;
	}

	uiComponents.push_back(uiComponent_);
}

void Scene::RemoveUIComponent(UIComponent* const uiComponent_)
{
	uiComponents.erase(std::remove(uiComponents.begin(), uiComponents.end(), uiComponent_), uiComponents.end());
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

	for (const std::unique_ptr<GameObject>& object : objects)
	{
		object->Update();
	}

	objects.erase(std::remove_if(objects.begin(), objects.end(),
		[](const std::unique_ptr<GameObject>& object)
		{
			return object->IsDestroyed();
		}), objects.end());
}

void Scene::Render(RenderTarget& renderTarget_)
{
	if (!isLoaded)
	{
		return;
	}

	if (nullptr != mainCamera)
	{
		renderTarget_.SetCamera(*mainCamera);
	}

	OnRender(renderTarget_);

	for (const std::unique_ptr<GameObject>& object : objects)
	{
		object->Render(renderTarget_, false);
	}

	RenderInstancedMeshes(renderTarget_);

	for (UIComponent* const uiComponent : uiComponents)
	{
		if (nullptr != uiComponent && uiComponent->IsEnabled())
		{
			uiComponent->RenderUI(renderTarget_);
		}
	}
}

void Scene::RenderInstancedMeshes(RenderTarget& renderTarget_)
{
	std::unordered_map<MeshBatchKey, std::vector<Matrix4x4>, MeshBatchKeyHash> batches;

	for (const std::unique_ptr<GameObject>& object : objects)
	{
		if (nullptr == object || !object->IsActive())
		{
			continue;
		}

		MeshRenderer* const meshRenderer{ object->GetComponent<MeshRenderer>() };
		if (nullptr == meshRenderer || !meshRenderer->IsEnabled())
		{
			continue;
		}

		const Mesh* const mesh{ meshRenderer->GetMesh() };
		if (nullptr == mesh)
		{
			continue;
		}

		const Transform* const transform{ object->GetTransform() };
		if (nullptr == transform)
		{
			continue;
		}

		const Material* const material{ meshRenderer->GetMaterial() };
		batches[MeshBatchKey{ mesh, material }].push_back(transform->GetWorldMatrix());
	}

	for (const auto& [key, worldMatrices] : batches)
	{
		if (nullptr == key.mesh || worldMatrices.empty())
		{
			continue;
		}

		renderTarget_.SetMaterial(nullptr != key.material ? *key.material : Material::GetDefault());
		renderTarget_.DrawMeshInstanced(*key.mesh, worldMatrices);
	}
}

void Scene::Unload()
{
	if (!isLoaded)
	{
		return;
	}

	OnUnload();

	cameras.clear();
	uiComponents.clear();
	mainCamera = nullptr;
	objects.clear();
	isLoaded = false;
}
