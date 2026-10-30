#include "Precompiled.hpp"
#include "Entity.hpp"

namespace TUK::Framework
{
	void Entity::RemoveDestroyedComponents()
	{
		const auto destroyedCount = std::ranges::count_if(components,
			[](const auto& component) { return component->isDestroyed; });
		if (destroyedCount == 0)
		{
			return;
		}

		// 삭제 콜백이 컴포넌트를 추가하더라도 원래 컨테이너의 순회에 영향을 주지 않는다.
		std::vector<std::unique_ptr<Component>> destroyed;
		destroyed.reserve(static_cast<std::size_t>(destroyedCount));
		std::erase_if(componentsByType, [](const auto& entry) { return entry.second->isDestroyed; });
		std::size_t remainingCount = 0;
		for (std::size_t index = 0; index < components.size(); ++index)
		{
			if (components[index]->isDestroyed)
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
			if (component->isEnabled)
			{
				component->isEnabled = false;
				component->OnDisable();
			}
			component->OnDestroy();
		}
	}

	bool Entity::StartComponent(Component& component)
	{
		if (!component.isEnabled || component.isDestroyed)
		{
			return false;
		}
		if (!component.isStared)
		{
			component.isStared = true;
			component.OnStart();
		}
		return component.isEnabled && !component.isDestroyed;
	}

	void Entity::FixedUpdate()
	{
		RemoveDestroyedComponents();
		const std::size_t count = components.size();
		for (std::size_t index = 0; index < count; ++index)
		{
			Component* component = components[index].get();
			if (StartComponent(*component))
			{
				component->OnFixedUpdate();
			}
		}
	}

	void Entity::Update()
	{
		RemoveDestroyedComponents();
		const std::size_t count = components.size();
		for (std::size_t index = 0; index < count; ++index)
		{
			Component* component = components[index].get();
			if (StartComponent(*component))
			{
				component->OnUpdate();
			}
		}
	}

	void Entity::PreRender(D3DCommandBuffer& commandBuffer)
	{
		const std::size_t count = components.size();
		for (std::size_t index = 0; index < count; ++index)
		{
			Component* component = components[index].get();
			if (component->isStared && component->isEnabled && !component->isDestroyed)
			{
				component->OnPreRender(commandBuffer);
			}
		}
	}

	void Entity::Render(D3DCommandBuffer& commandBuffer)
	{
		const std::size_t count = components.size();
		for (std::size_t index = 0; index < count; ++index)
		{
			Component* component = components[index].get();
			if (component->isStared && component->isEnabled && !component->isDestroyed)
			{
				component->OnRender(commandBuffer);
			}
		}
	}

	void Entity::PostRender(D3DCommandBuffer& commandBuffer)
	{
		const std::size_t count = components.size();
		for (std::size_t index = 0; index < count; ++index)
		{
			Component* component = components[index].get();
			if (component->isStared && component->isEnabled && !component->isDestroyed)
			{
				component->OnPostRender(commandBuffer);
			}
		}
	}

	void Entity::LateUpdate()
	{
		RemoveDestroyedComponents();
		const std::size_t count = components.size();
		for (std::size_t index = 0; index < count; ++index)
		{
			Component* component = components[index].get();
			if (StartComponent(*component))
			{
				component->OnLateUpdate();
			}
		}
	}
}
