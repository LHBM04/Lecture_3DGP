#pragma once

class GameObject;

class Component
{
	friend class Scene;

public:
	Component() = default;
	virtual ~Component() = default;

	// 복사 금지.
	Component(const Component&) = delete;
	Component& operator=(const Component&) = delete;
	
	// 이동 금지.
	Component(Component&&) = delete;
	Component& operator=(Component&&) = delete;

	[[nodiscard]] GameObject* GetOwner() noexcept;
	[[nodiscard]] const GameObject* GetOwner() const noexcept;

protected:
	virtual void OnAwake() noexcept {};
	virtual void OnEnable() noexcept {};
	virtual void OnStart() noexcept {};

	virtual void OnUpdate(float deltaTime_) noexcept {};
	virtual void OnFixedUpdate(float fixedDeltaTime_) noexcept {};
	virtual void OnLateUpdate(float deltaTime_) noexcept {};

	virtual void OnPreRender() noexcept {};
	virtual void OnRender() noexcept {};
	virtual void OnPostRender() noexcept {};

	virtual void OnDisable() noexcept {};
	virtual void OnDestroy() noexcept {};

private:
	GameObject* owner{ nullptr };
};

inline GameObject* Component::GetOwner() noexcept
{
	return owner;
}

inline const GameObject* Component::GetOwner() const noexcept
{
	return owner;
}
