#include "Precompiled.hpp"
#include "Entity.hpp"

#include <algorithm>
#include <ranges>

namespace TUK::Framework
{
	Entity::~Entity() noexcept
	{
		DestroyAllComponents();
	}

	void Entity::RemoveDestroyedComponents()
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
