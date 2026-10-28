#include "Precompiled.hpp"
#include "Object.hpp"

#include "Scene.hpp"

Object::~Object()
{
	for (auto& [typeIndex, component] : components)
	{
		(void)typeIndex;
		if (component)
		{
			component->Detach();
		}
	}
}

Transform* Object::GetTransform()
{
	return transform;
}

const Transform* Object::GetTransform() const
{
	return transform;
}

Scene* Object::GetCurrentScene() const
{
	return currentScene;
}

void Object::SetCurrentScene(Scene* scene_)
{
	currentScene = scene_;
}

void Object::Update()
{
	for (const std::unique_ptr<Component>& component : components | std::views::values)
	{
		if (component->IsEnabled())
		{
			component->Update();
		}
	}
}

void Object::Render()
{
	for (const std::unique_ptr<Component>& component : components | std::views::values)
	{
		if (component->IsEnabled())
		{
			component->Render();
		}
	}
}

const std::string& Object::GetName() const
{
	return name;
}

void Object::SetName(const std::string& name_)
{
	name = name_;
}

const std::string& Object::GetTag() const
{
	return tag;
}

void Object::SetTag(const std::string& tag_)
{
	tag = tag_;
}

bool Object::IsActive() const
{
	return isActive;
}

void Object::SetActive(bool active_)
{
	isActive = active_;
}

bool Object::IsDestroyed() const
{
	return isDestroyed;
}

void Object::Destroy()
{
	isDestroyed = true;
	isActive = false;
}
