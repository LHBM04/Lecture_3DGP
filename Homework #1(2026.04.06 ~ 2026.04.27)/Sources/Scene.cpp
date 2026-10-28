#include "Precompiled.hpp"
#include "Scene.hpp"

#include "Object.hpp"
#include "Renderer.hpp"
#include "Transform.hpp"
#include "Vector3D.hpp"

const std::vector<Camera*>& Scene::GetCameras() const
{
	return cameras;
}

void Scene::AddCamera(Camera* camera_)
{
	cameras.push_back(camera_);
}

void Scene::RemoveCamera(Camera* camera_)
{
	cameras.erase(std::remove(cameras.begin(), cameras.end(), camera_), cameras.end());
}

const std::vector<std::unique_ptr<Object>>& Scene::GetObjects() const
{
	return objects;
}

Object* Scene::AddObject()
{
	std::unique_ptr<Object> newObject = std::make_unique<Object>();
	newObject->SetCurrentScene(this);
	newObject->SetName("New Object");
	newObject->SetTag("Untagged");

	Transform* transform = newObject->AddComponent<Transform>();
	transform->SetWorldPosition(Vector3D::GetZero());
	transform->SetWorldRotation(Quaternion::GetIdentity());
	transform->SetWorldScale(Vector3D::GetOne());

	objects.push_back(std::move(newObject));
	return objects.back().get();
}

void Scene::RemoveObject(Object* object_)
{
	objects.erase(std::remove_if(objects.begin(), objects.end(),
		[object_](const std::unique_ptr<Object>& object)
		{
			return object.get() == object_;
		}), objects.end());
}

void Scene::Load()
{
	OnLoad();
}

void Scene::Update()
{
	for (auto iter = objects.begin(); iter != objects.end();)
	{
		if ((*iter)->IsDestroyed())
		{
			iter = objects.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	const size_t updateCount = objects.size();
	for (size_t i = 0; i < updateCount; ++i)
	{
		Object* object = objects[i].get();
		if (object && object->IsActive())
		{
			object->Update();
		}
	}

	OnUpdate();
}

void Scene::Render()
{
	for (Camera* camera : cameras)
	{
		Renderer::SetCamera(camera);

		for (const std::unique_ptr<Object>& object : objects)
		{
			if (object->IsActive())
			{
				Renderer::SetObject(object.get());
				object->Render();
			}
		}
	}

	OnRender();
}

void Scene::Unload()
{
	OnUnload();

	for (const std::unique_ptr<Object>& object : objects)
	{
		object->Destroy();
	}
}
