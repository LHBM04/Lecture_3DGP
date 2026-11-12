#include "Precompiled.hpp"
#include "GameObject.hpp"

#include "../Core/Common.hpp"

#include "Scene.hpp"

namespace TUK::Framework
{
	GameObject::~GameObject() noexcept
	{
		DestroyAllComponents();
	}

	void GameObject::FixedUpdate()
	{
		UpdateComponents(&Component::OnFixedUpdate);
	}

	void GameObject::Update()
	{
		UpdateComponents(&Component::OnUpdate);
	}

	void GameObject::LateUpdate()
	{
		UpdateComponents(&Component::OnLateUpdate);
	}

	void GameObject::PreRender(GPUCommandBuffer& commandBuffer)
	{
		RenderComponents(&Component::OnPreRender, commandBuffer);
	}

	void GameObject::Render(GPUCommandBuffer& commandBuffer)
	{
		RenderComponents(&Component::OnRender, commandBuffer);
	}

	void GameObject::PostRender(GPUCommandBuffer& commandBuffer)
	{
		RenderComponents(&Component::OnPostRender, commandBuffer);
	}

#ifdef _DEBUG
	void GameObject::DrawGizmos(GPUCommandBuffer& commandBuffer)
	{
		RenderComponents(&Component::OnDrawGizmos, commandBuffer);
	}
#endif

	void GameObject::UpdateComponents(ComponentCallback callback)
	{
		ASSERT_FMT(callback, "callback이 비어 있습니다.");

		if (!scene || !scene->IsEntered() || isDestroyed)
		{
			return;
		}

		DestroyComponents();

		const usize count = components.size();
		for (usize index = 0; index < count && scene->IsEntered() && isEnabled && !isDestroyed; ++index)
		{
			Component* component = components[index].get();
			if (!component->IsEnabled() || component->IsDestroyed())
			{
				continue;
			}

			if (!component->IsStarted())
			{
				component->SetStarted(true);
				component->OnStart();
				if (!scene->IsEntered() || !isEnabled || isDestroyed ||
					!component->IsEnabled() || component->IsDestroyed())
				{
					continue;
				}
			}

			(component->*callback)();
		}
	}

	void GameObject::RenderComponents(RenderCallback callback, GPUCommandBuffer& commandBuffer)
	{
		ASSERT_FMT(callback, "callback이 비어 있습니다.");

		const usize count = components.size();
		for (usize index = 0; index < count && scene->IsEntered() && isEnabled && !isDestroyed; ++index)
		{
			Component* component = components[index].get();
			if (component->IsStarted() && component->IsEnabled() && !component->IsDestroyed())
			{
				(component->*callback)(commandBuffer);
			}
		}
	}

	void GameObject::DestroyComponents()
	{
		const auto destroyedCount = std::ranges::count_if(components,
			[](const auto& component)
			{
				return component->IsDestroyed();
			});
		if (destroyedCount == 0)
		{
			return;
		}

		std::vector<std::unique_ptr<Component>> destroyed;
		destroyed.reserve(static_cast<usize>(destroyedCount));
		std::erase_if(componentsByType,
			[](const auto& entry)
			{
				return entry.second->IsDestroyed();
			});

		usize remainingCount = 0;
		for (usize index = 0; index < components.size(); ++index)
		{
			if (components[index]->IsDestroyed())
			{
				destroyed.push_back(std::move(components[index]));
			}
			else
			{
				if (remainingCount != index)
				{
					components[remainingCount] = std::move(components[index]);
				}
				++remainingCount;
			}
		}
		components.resize(remainingCount);

		for (const auto& component : components)
		{
			componentsByType[std::type_index(typeid(*component))] = component.get();
		}

		for (const auto& component : destroyed)
		{
			if (component->IsEnabled())
			{
				component->OnDisable();
			}
			component->OnDestroy();
		}
	}

	void GameObject::DestroyAllComponents()
	{
		if (components.empty())
		{
			return;
		}

		componentsByType.clear();
		std::vector<std::unique_ptr<Component>> removing = std::move(components);
		components.clear();

		for (const auto& component : removing | std::views::reverse)
		{
			const bool wasEnabled = component->IsEnabled();
			component->Destroy();
			if (wasEnabled)
			{
				component->OnDisable();
			}
			component->OnDestroy();
		}
	}
}
