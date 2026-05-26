#include "Precompiled.h"
#include "GameObject.h"

namespace
{
	template <typename T>
	[[nodiscard]] Component* TryGetActiveComponent(T* role_) noexcept
	{
		if (nullptr == role_)
		{
			return nullptr;
		}

		Component* component{ dynamic_cast<Component*>(role_) };
		if (nullptr == component || component->IsDestroyed() || !component->IsEnabled())
		{
			return nullptr;
		}

		return component;
	}
}

void GameObject::Update(const TimeContext& context_)
{
	for (Updatable* updatable : updatables)
	{
		if (nullptr == TryGetActiveComponent(updatable))
		{
			continue;
		}

		updatable->OnUpdate(context_);
	}
}

void GameObject::FixedUpdate(const TimeContext& context_)
{
	for (const std::unique_ptr<Component>& component : components | std::views::values)
	{
		component->TickFixedUpdate(context_);
	}
}

void GameObject::Render(RenderContext& context_)
{
	for (Renderable* renderable : renderables)
	{
		if (nullptr == TryGetActiveComponent(renderable))
		{
			continue;
		}

		renderable->OnRender(context_);
	}
}

void GameObject::RenderUI(RenderContext& context_)
{
	for (RenderableUI* renderable : uiRenderables)
	{
		if (nullptr == TryGetActiveComponent(renderable))
		{
			continue;
		}

		renderable->OnRenderUI(context_);
	}
}

void GameObject::DispatchInput(const InputContext& context_)
{
	for (InputHandler* handler : inputHandlers)
	{
		if (nullptr == handler)
		{
			continue;
		}

		Component* component{ dynamic_cast<Component*>(handler) };
		if (nullptr == component || component->IsDestroyed() || !component->IsEnabled())
		{
			continue;
		}

		switch (context_.GetPhase())
		{
		case InputPhase::Started:
			handler->OnInputStarted(context_);
			break;

		case InputPhase::Performed:
			handler->OnInputPerformed(context_);
			break;

		case InputPhase::Canceled:
			handler->OnInputCanceled(context_);
			break;
		}
	}
}

void GameObject::NotifyCollisionEnter(GameObject& other_)
{
	for (Collidable* collidable : collidables)
	{
		if (nullptr != TryGetActiveComponent(collidable))
		{
			collidable->OnCollisionEnter(other_);
		}
	}
}

void GameObject::NotifyCollisionStay(GameObject& other_)
{
	for (Collidable* collidable : collidables)
	{
		if (nullptr != TryGetActiveComponent(collidable))
		{
			collidable->OnCollisionStay(other_);
		}
	}
}

void GameObject::NotifyCollisionExit(GameObject& other_)
{
	for (Collidable* collidable : collidables)
	{
		if (nullptr != collidable)
		{
			collidable->OnCollisionExit(other_);
		}
	}
}

bool GameObject::HasCollisionListeners() const noexcept
{
	return !collidables.empty();
}

std::span<SceneTransitionRequest* const> GameObject::GetSceneTransitionRequests() noexcept
{
	return sceneTransitionRequests;
}

Scene* GameObject::GetCurrentScene()
{
	return currentScene;
}

const Scene* GameObject::GetCurrentScene() const
{
	return currentScene;
}

const std::string& GameObject::GetName() const
{
	return name;
}

void GameObject::SetName(const std::string& name_)
{
	name = name_;
}

const std::string& GameObject::GetTag() const
{
	return tag;
}

void GameObject::SetTag(const std::string& tag_)
{
	tag = tag_;
}

bool GameObject::IsActive() const
{
	return isActive;
}

void GameObject::SetActive(bool active_)
{
	isActive = active_;
}

bool GameObject::IsDestroyed() const
{
	return isDestroyed;
}

void GameObject::Destroy()
{
	isDestroyed = true;
	isActive = false;
}
