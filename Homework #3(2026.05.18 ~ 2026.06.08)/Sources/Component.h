#pragma once

class Collider;
class GameObject;

class Component
{
	friend class GameObject;

public:
	Component() = default;
	virtual ~Component() = default;

	[[nodiscard]] GameObject* GetOwner() noexcept;
	[[nodiscard]] const GameObject* GetOwner() const noexcept;

protected:
	virtual void OnAwake() {}
	virtual void OnEnable() {}
	virtual void OnStart() {}

	virtual void OnUpdate(float deltaTime_) {}
	virtual void OnLateUpdate(float deltaTime_) {}
	virtual void OnFixedUpdate(float fixedDeltaTime_) {}

	virtual void OnDisable() {}
	virtual void OnDestroy() {}

	virtual void OnCollisionEnter(Collider* other_) {}
	virtual void OnCollisionStay(Collider* other_) {}
	virtual void OnCollisionExit(Collider* other_) {}

private:
	Component(const Component&) = delete;
	Component& operator=(const Component&) = delete;

	Component(Component&&) = delete;
	Component& operator=(Component&&) = delete;

	GameObject* owner{ nullptr };
};
