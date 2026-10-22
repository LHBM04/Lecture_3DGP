#pragma once

#include <concepts>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>

#include "Component.hpp"

namespace TUK::Framework
{
	class World;
	class CommandBuffer;

	class Entity
	{
		friend class World;

	public:
		Entity() noexcept;
		virtual ~Entity() noexcept;
		
		/** 복사 금지 */
		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;

		/** 이동 금지 */
		Entity(Entity&&) = delete;
		Entity& operator=(Entity&&) = delete;

		/** 이름 Get/Setter */
		const std::string& GetName() const noexcept;
		void SetName(std::string_view name);

		/** 태그 Get/Setter */
		const std::string& GetTag() const noexcept;
		void SetTag(std::string_view tag);

		/** 컴포넌트 추가 */
		template <std::derived_from<Component> TComponent>
		TComponent* AddComponent();

		/** 컴포넌트 가져오기 */
		template <std::derived_from<Component> TComponent>
		[[nodiscard]] TComponent* GetComponent();

		/** 컴포넌트 가져오기 (const) */
		template <std::derived_from<Component> TComponent>
		[[nodiscard]] const TComponent* GetComponent() const;

	protected:
		void FixedUpdate();
		void Update();
		void LateUpdate();
		void PreRender(CommandBuffer& renderer);
		void Render(CommandBuffer& renderer);
		void PostRender(CommandBuffer& renderer);

	private:
		void RemoveDestroyedComponents();
		bool StartComponent(Component& component);

		std::string name;
		std::string tag;

		std::vector<std::unique_ptr<Component>> components;
		std::unordered_map<std::type_index, Component*> componentsByType;

		World* world = nullptr;
	};

	template <std::derived_from<Component> TComponent>
	TComponent* Entity::AddComponent()
	{
		auto component = std::make_unique<TComponent>();
		component->owner = this;

		auto componentPtr = component.get();
		components.push_back(std::move(component));
		try
		{
			componentsByType[std::type_index(typeid(TComponent))] = componentPtr;
		}
		catch (...)
		{
			components.pop_back();
			throw;
		}
		return componentPtr;
	}

	template <std::derived_from<Component> TComponent>
	TComponent* Entity::GetComponent()
	{
		auto result = componentsByType.find(typeid(TComponent));
		if (result != componentsByType.end() && !result->second->isDestroyed)
		{
			return static_cast<TComponent*>(result->second);
		}

		return nullptr;
	}

	template <std::derived_from<Component> TComponent>
	const TComponent* Entity::GetComponent() const
	{
		auto result = componentsByType.find(typeid(TComponent));
		if (result != componentsByType.end() && !result->second->isDestroyed)
		{
			return static_cast<const TComponent*>(result->second);
		}

		return nullptr;
	}
}
