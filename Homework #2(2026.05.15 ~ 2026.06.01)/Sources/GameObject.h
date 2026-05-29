#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>

#include "Camera.h"
#include "CubeCollider.h"
#include "Light.h"
#include "MeshComponent.h"
#include "SphereCollider.h"
#include "Transform.h"

class Scene;

using ComponentTuple = std::tuple<
	std::unique_ptr<Transform>,
	std::unique_ptr<Camera>,
	std::unique_ptr<Light>,
	std::unique_ptr<MeshComponent>,
	std::unique_ptr<SphereCollider>,
	std::unique_ptr<CubeCollider>
>;

class GameObject final
{
	friend class Scene;

public:
	GameObject() = default;
	~GameObject() = default;

	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;

	GameObject(GameObject&&) = delete;
	GameObject& operator=(GameObject&&) = delete;

	void Update(float deltaTime_) noexcept;
	void FixedUpdate(float fixedDeltaTime_) noexcept;
	void LateUpdate(float deltaTime_) noexcept;
	void Render() noexcept;

	[[nodiscard]] std::wstring_view GetName() const noexcept;
	void SetName(std::wstring_view name_);

	[[nodiscard]] std::wstring_view GetTag() const noexcept;
	void SetTag(std::wstring_view tag_);

	[[nodiscard]] bool IsActive() const noexcept;
	void SetActive(bool isActive_);

	[[nodiscard]] bool IsDestroyed() const noexcept;
	void Destroy();

	[[nodiscard]] Scene* GetScene() const noexcept;

	template <class TComponent>
	TComponent* AddComponent() noexcept;

	template <class TComponent>
	[[nodiscard]] TComponent* GetComponent() noexcept;

	template <class TComponent>
	[[nodiscard]] const TComponent* GetComponent() const noexcept;

private:
	std::wstring name;
	std::wstring tag;

	bool isActive{ true };
	bool isDestroyed{ false };

	Scene* scene{ nullptr };

	ComponentTuple components;
};

template <class TComponent>
inline TComponent* GameObject::AddComponent() noexcept
{
	auto& ptr = std::get<std::unique_ptr<TComponent>>(components);
	if (!ptr)
	{
		ptr = std::make_unique<TComponent>();
		ptr->SetOwner(this);
		ptr->Awake();
		if (isActive)
		{
			ptr->Enable();
		}
	}
	return ptr.get();
}

template <class TComponent>
inline TComponent* GameObject::GetComponent() noexcept
{
	return std::get<std::unique_ptr<TComponent>>(components).get();
}

template <class TComponent>
inline const TComponent* GameObject::GetComponent() const noexcept
{
	return std::get<std::unique_ptr<TComponent>>(components).get();
}
