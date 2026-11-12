#pragma once

#include "../Core/Common.hpp"

#include "Component.hpp"

namespace TUK::Framework
{
	class Scene;

	class GameObject
	{
	public:
		explicit GameObject(Scene* scene) noexcept;
		explicit GameObject(Scene* scene, std::string_view name) noexcept;
		explicit GameObject(Scene* scene, std::string_view name, std::string_view tag) noexcept;
		virtual ~GameObject() noexcept;

		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;

		GameObject(GameObject&&) noexcept = default;
		GameObject& operator=(GameObject&&) noexcept = default;

		void FixedUpdate();
		void Update();
		void LateUpdate();
		void PreRender(GPUCommandBuffer& commandBuffer);
		void Render(GPUCommandBuffer& commandBuffer);
		void PostRender(GPUCommandBuffer& commandBuffer);
#ifdef _DEBUG
		void DrawGizmos(GPUCommandBuffer& commandBuffer);
#endif

		[[nodiscard]] const std::string& GetName() const noexcept;
		void SetName(std::string_view value) noexcept;

		[[nodiscard]] const std::string& GetTag() const noexcept;
		void SetTag(std::string_view value) noexcept;

		[[nodiscard]] bool CompareTag(std::string_view value) const noexcept;

		[[nodiscard]] const Scene* GetScene() const noexcept;
		[[nodiscard]] Scene* GetScene() noexcept;

		[[nodiscard]] bool IsEnabled() const noexcept;
		void SetEnabled(bool value) noexcept;

		[[nodiscard]] bool ShouldDontDestoryOnLoad() const noexcept;
		void SetDontDestroyOnLoad(bool value) noexcept;

		[[nodiscard]] bool IsDestroyed() const noexcept;
		void Destroy() noexcept;

		template <std::derived_from<Component> TComponent>
		TComponent* AddComponent();

		template <std::derived_from<Component> TComponent>
		[[nodiscard]] TComponent* GetComponent() noexcept;

		template <std::derived_from<Component> TComponent>
		[[nodiscard]] const TComponent* GetComponent() const noexcept;

		template <std::derived_from<Component> TComponent>
		[[nodiscard]] bool HasComponent() const noexcept;

		template <std::derived_from<Component> TComponent>
		bool RemoveComponent() noexcept;

		[[nodiscard]] auto GetComponents() noexcept;
		[[nodiscard]] auto GetComponents() const noexcept;

	private:
		friend class Scene;

		using ComponentCallback = void (Component::*)();
		using RenderCallback = void (Component::*)(GPUCommandBuffer&);

		void UpdateComponents(ComponentCallback callback);
		void RenderComponents(RenderCallback callback, GPUCommandBuffer& commandBuffer);
		void DestroyComponents();
		void DestroyAllComponents();

		std::string name{ "New GameObject" };
		std::string tag{ "Untagged" };

		std::vector<std::unique_ptr<Component>> components;
		std::unordered_map<std::type_index, Component*> componentsByType;

		Scene* scene = nullptr;

		bool isEnabled = true;
		bool dontDestroyOnLoad = false;
		bool isDestroyed = false;
	};

	inline GameObject::GameObject(Scene* scene) noexcept
		: scene(scene)
	{
		ASSERT_FMT(scene, "엔티티의 월드가 nullptr입니다.");
	}

	inline GameObject::GameObject(Scene* scene, std::string_view name) noexcept
		: name(name)
		, scene(scene)
	{
		ASSERT_FMT(scene, "엔티티의 월드가 nullptr입니다.");
	}

	inline GameObject::GameObject(Scene* scene, std::string_view name, std::string_view tag) noexcept
		: name(name)
		, tag(tag)
		, scene(scene)
	{
		ASSERT_FMT(scene, "엔티티의 월드가 nullptr입니다.");
	}

	inline const std::string& GameObject::GetName() const noexcept
	{
		return name;
	}

	inline void GameObject::SetName(std::string_view value) noexcept
	{
		name = value;
	}

	inline const std::string& GameObject::GetTag() const noexcept
	{
		return tag;
	}

	inline void GameObject::SetTag(std::string_view value) noexcept
	{
		tag = value;
	}

	inline bool GameObject::CompareTag(std::string_view value) const noexcept
	{
		return std::string_view(tag) == value;
	}

	inline const Scene* GameObject::GetScene() const noexcept
	{
		return scene;
	}

	inline Scene* GameObject::GetScene() noexcept
	{
		return scene;
	}

	inline bool GameObject::IsEnabled() const noexcept
	{
		return isEnabled;
	}

	inline void GameObject::SetEnabled(bool value) noexcept
	{
		isEnabled = value;
	}

	inline bool GameObject::ShouldDontDestoryOnLoad() const noexcept
	{
		return dontDestroyOnLoad;
	}

	inline void GameObject::SetDontDestroyOnLoad(bool value) noexcept
	{
		dontDestroyOnLoad = value;
	}

	inline bool GameObject::IsDestroyed() const noexcept
	{
		return isDestroyed;
	}

	inline void GameObject::Destroy() noexcept
	{
		isDestroyed = true;
	}

	template <std::derived_from<Component> TComponent>
	TComponent* GameObject::AddComponent()
	{
		auto component = std::make_unique<TComponent>(this);
		TComponent* componentPtr = component.get();

		components.push_back(std::move(component));
		componentsByType[std::type_index(typeid(TComponent))] = componentPtr;

		Component& addedComponent = *componentPtr;
		addedComponent.OnAwake();
		if (addedComponent.IsEnabled() && !addedComponent.IsDestroyed())
		{
			addedComponent.OnEnable();
		}

		return componentPtr;
	}

	template <std::derived_from<Component> TComponent>
	TComponent* GameObject::GetComponent() noexcept
	{
		const auto result = componentsByType.find(std::type_index(typeid(TComponent)));
		if (result != componentsByType.end() && !result->second->IsDestroyed())
		{
			return static_cast<TComponent*>(result->second);
		}

		return nullptr;
	}

	template <std::derived_from<Component> TComponent>
	const TComponent* GameObject::GetComponent() const noexcept
	{
		const auto result = componentsByType.find(std::type_index(typeid(TComponent)));
		if (result != componentsByType.end() && !result->second->IsDestroyed())
		{
			return static_cast<const TComponent*>(result->second);
		}

		return nullptr;
	}

	template <std::derived_from<Component> TComponent>
	bool GameObject::HasComponent() const noexcept
	{
		return GetComponent<TComponent>() != nullptr;
	}

	template <std::derived_from<Component> TComponent>
	bool GameObject::RemoveComponent() noexcept
	{
		TComponent* component = GetComponent<TComponent>();
		if (!component)
		{
			return false;
		}

		component->Destroy();
		return true;
	}

	inline auto GameObject::GetComponents() noexcept
	{
		return components | std::views::transform([](const std::unique_ptr<Component>& componentPtr) -> Component*
			{
				return componentPtr.get();
			});
	}

	inline auto GameObject::GetComponents() const noexcept
	{
		return components | std::views::transform([](const std::unique_ptr<Component>& componentPtr) -> const Component*
			{
				return componentPtr.get();
			});
	}
}
