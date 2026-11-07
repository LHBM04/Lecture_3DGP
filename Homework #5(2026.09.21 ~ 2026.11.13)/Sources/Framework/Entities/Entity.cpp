#include "Precompiled.hpp"
#include "Entity.hpp"

#include <algorithm>
#include <ranges>

#include "World.hpp"

namespace TUK::Framework
{
	Entity::~Entity() noexcept
	{
		DestroyAllComponents();
	}

	void Entity::FixedUpdate()
	{
		UpdateComponents(&Component::OnFixedUpdate);
	}

	void Entity::Update()
	{
		UpdateComponents(&Component::OnUpdate);
	}

	void Entity::LateUpdate()
	{
		UpdateComponents(&Component::OnLateUpdate);
	}

	void Entity::PreRender(GPUCommandBuffer& commandBuffer)
	{
		RenderComponents(&Component::OnPreRender, commandBuffer);
	}

	void Entity::Render(GPUCommandBuffer& commandBuffer)
	{
		RenderComponents(&Component::OnRender, commandBuffer);
	}

	void Entity::PostRender(GPUCommandBuffer& commandBuffer)
	{
		RenderComponents(&Component::OnPostRender, commandBuffer);
	}

	void Entity::UpdateComponents(ComponentCallback callback)
	{
		if (!world || !world->IsEntered() || isDestroyed)
		{
			return;
		}

		DestroyComponents();

		// 콜백에서 추가한 컴포넌트는 다음 갱신 단계부터 실행합니다.
		const usize count = components.size();
		for (usize index = 0; index < count && world->IsEntered() && isEnabled && !isDestroyed; ++index)
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
				if (!world->IsEntered() || !isEnabled || isDestroyed ||
					!component->IsEnabled() || component->IsDestroyed())
				{
					continue;
				}
			}

			callback(*component);
		}
	}

	void Entity::RenderComponents(RenderCallback callback, GPUCommandBuffer& commandBuffer)
	{
		const usize count = components.size();
		for (usize index = 0; index < count && world->IsEntered() && isEnabled && !isDestroyed; ++index)
		{
			Component* component = components[index].get();
			if (component->IsStarted() && component->IsEnabled() && !component->IsDestroyed())
			{
				callback(*component, commandBuffer);
			}
		}
	}

	void Entity::DestroyComponents()
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

		// 삭제 콜백이 컴포넌트를 추가하더라도 원래 컨테이너의 순회에 영향을 주지 않는다.
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

		// 같은 타입이 여러 개라면 남아 있는 마지막 컴포넌트를 조회 대상으로 사용한다.
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

	void Entity::DestroyAllComponents()
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
