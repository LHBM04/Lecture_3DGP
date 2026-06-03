#include "Precompiled.h"
#include "GameObject.h"

Scene* GameObject::GetScene() const noexcept
{
	return scene;
}

void GameObject::SetScene(Scene* scene_) noexcept
{
	scene = scene_;
}
