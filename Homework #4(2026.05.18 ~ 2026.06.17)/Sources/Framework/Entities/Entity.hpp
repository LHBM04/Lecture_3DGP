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
	class D3DCommandBuffer;

	class Entity
	{
		friend class World;

	public:
		explicit Entity(std::string_view name, std::string_view tag, World& world) noexcept;
		virtual ~Entity() noexcept = default;
		
		/** 복사 금지 */
		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;

		/** 이동 금지 */
		Entity(Entity&&) = default;
		Entity& operator=(Entity&&) = default;

		[[nodiscard]] const std::string& GetName() const noexcept;
		void SetName(std::string_view value);

		[[nodiscard]] const std::string& GetTag() const noexcept;
		void SetTag(std::string_view value);

		[[nodiscard]] const World& GetWorld() const noexcept;
		[[nodiscard]] World& GetWorld() noexcept;

		template <std::derived_from<Component> TComponent>
		TComponent* AddComponent();

		template <std::derived_from<Component> TComponent>
		[[nodiscard]] TComponent* GetComponent();

		template <std::derived_from<Component> TComponent>
		[[nodiscard]] const TComponent* GetComponent() const;

	protected:
		void FixedUpdate();
		void Update();
		void LateUpdate();
		void PreRender(D3DCommandBuffer& commandBuffer);
		void Render(D3DCommandBuffer& commandBuffer);
		void PostRender(D3DCommandBuffer& commandBuffer);

	private:
		void RemoveDestroyedComponents();
		bool StartComponent(Component& component);

		std::string name;
		std::string tag;

		std::vector<std::unique_ptr<Component>> components;
		std::unordered_map<std::type_index, Component*> componentsByType;

		World& world;
	};

	inline Entity::Entity(std::string_view name, std::string_view tag, World& world) noexcept
		: name(name)
		, tag(tag)
		, world(world)
	{
	}

	inline const std::string& Entity::GetName() const noexcept
	{
		return name;
	}

	inline void Entity::SetName(std::string_view value)
	{
		name = value;
	}

	inline const std::string& Entity::GetTag() const noexcept
	{
		return tag;
	}

	inline void Entity::SetTag(std::string_view value)
	{
		tag = value;
	}

	inline const World& Entity::GetWorld() const noexcept
	{
		return world;
	}

	inline World& Entity::GetWorld() noexcept
	{
		return world;
	}

	template <std::derived_from<Component> TComponent>
	TComponent* Entity::AddComponent()
	{
		auto component = std::make_unique<TComponent>(*this);
		auto componentPtr = component.get();

		components.push_back(std::move(component));
		componentsByType[std::type_index(typeid(TComponent))] = componentPtr;

		Component& addedComponent = *componentPtr;
		addedComponent.OnAwake();
		if (addedComponent.isEnabled && !addedComponent.isDestroyed)
		{
			addedComponent.OnEnable();
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
