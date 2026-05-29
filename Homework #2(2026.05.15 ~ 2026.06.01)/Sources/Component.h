#pragma once

class GameObject;

class Component
{
	friend class Scene;
	friend class GameObject;

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

	[[nodiscard]] bool IsStarted() const noexcept;
	
	[[nodiscard]] bool IsActive() const noexcept;
	void Enable();

protected:
	virtual void OnAwake() {};
	virtual void OnEnable() {};
	virtual void OnStart() {};

	virtual void OnUpdate(float deltaTime_) {};
	virtual void OnFixedUpdate(float fixedDeltaTime_) {};
	virtual void OnLateUpdate(float deltaTime_) {};

	// virtual void OnCollisionEnter() {};
	// virtual void OnCollisionStay() {};
	// virtual void OnCollisionExit() {};

	// virtual void OnTriggerEnter() {};
	// virtual void OnTriggerStay() {};
	// virtual void OnTriggerExit() {};

	virtual void OnPreRender() {};
	virtual void OnRender() {};
	virtual void OnPostRender() {};

	virtual void OnDisable() {};
	virtual void OnDestroy() {};

private:
	void Awake();

	void SetOwner(GameObject* owner_) noexcept;

	GameObject* owner{ nullptr };

	bool isStarted{ false };
	bool isEnabled{ true };
};

inline GameObject* Component::GetOwner() noexcept
{
	return owner;
}

inline const GameObject* Component::GetOwner() const noexcept
{
	return owner;
}

inline bool Component::IsStarted() const noexcept
{
	return isStarted;
}

inline bool Component::IsActive() const noexcept
{
	return isEnabled;
}

inline void Component::Awake()
{

}

inline void Component::Enable()
{

}

inline void Component::SetOwner(GameObject* owner_) noexcept
{
	owner = owner_;
}

inline void Scene::Update()
{
	if (!gameObject->IsActive())
	{
		return;
	}

	gameObject->Update(deltaTime_);
}

inline void GameObject::Update(float deltaTime_)
{
	if (!IsActive())
	{
		return;
	}

	// 컴포넌트 업데이트.
}
