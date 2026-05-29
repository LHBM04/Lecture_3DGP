#include "Precompiled.h"
#include "GameObject.h"

void GameObject::Update(float deltaTime_)
{
	if (!isActive)
	{
		return;
	}

	for (std::unique_ptr<Component>& component : components
		| std::views::values)
	{
		if (!component->isStarted)
		{
			component->OnStart();
			component->isStarted = true;
		}
	}

	for (std::unique_ptr<Component>& component : components
		| std::views::values)
	{
		component->OnUpdate(deltaTime_);
	}

	for (std::unique_ptr<Component>& component : components
		| std::views::values)
	{
		component->OnLateUpdate(deltaTime_);
	}
}

void GameObject::FixedUpdate(float fixedDeltaTime_)
{
	if (!isActive)
	{
		return;
	}

	for (std::unique_ptr<Component>& component : components 
		| std::views::values)
	{
		component->OnFixedUpdate(fixedDeltaTime_);
	}
}

void GameObject::Render()
{
	if (!isActive)
	{
		return;
	}

	for (std::unique_ptr<Component>& component : components
		| std::views::values)
	{
		component->OnPreRender();
	}

	for (std::unique_ptr<Component>& component : components
		| std::views::values)
	{
		component->OnRender();
	}

	for (std::unique_ptr<Component>& component : components
		| std::views::values)
	{
		component->OnPostRender();
	}
}
