#pragma once

class GameObject;

class Component
{
public:
	Component() = default;
	virtual ~Component()
	{
		Destroy();
	}

	// 복사 금지.
	Component(const Component&) = delete;
	Component& operator=(const Component&) = delete;
	
	// 이동 금지.
	Component(Component&&) = delete;
	Component& operator=(Component&&) = delete;

	[[nodiscard]] GameObject* GetOwner() const noexcept;
	void SetOwner(GameObject* owner_) noexcept;

	[[nodiscard]] bool IsStarted() const noexcept;
	
	[[nodiscard]] bool IsActive() const noexcept;
	[[nodiscard]] bool IsEnabled() const noexcept;
	void SetEnabled(bool isEnabled_);

	void Awake();
	void Enable();
	void Start();
	
	void Update(float deltaTime_);
	void FixedUpdate(float fixedDeltaTime_);
	void LateUpdate(float deltaTime_);
	
	void Render();
	
	void Disable();
	void Destroy();

protected:
	virtual void OnAwake() {}
	virtual void OnEnable() {}
	virtual void OnStart() {}

	virtual void OnUpdate([[maybe_unused]] float deltaTime_) {}
	virtual void OnFixedUpdate([[maybe_unused]] float fixedDeltaTime_) {}
	virtual void OnLateUpdate([[maybe_unused]] float deltaTime_) {}
	
	virtual void OnPreRender() {}
	virtual void OnRender() {}
	virtual void OnPostRender() {}
	
	virtual void OnDisable() {}
	virtual void OnDestroy() {}

private:
	GameObject* owner{ nullptr };
	bool isStarted{ false };
	bool isEnabled{ true };
};

inline GameObject* Component::GetOwner() const noexcept
{
	return owner;
}

inline void Component::SetOwner(GameObject* owner_) noexcept
{
	owner = owner_;
}

inline bool Component::IsStarted() const noexcept
{
	return isStarted;
}

inline bool Component::IsEnabled() const noexcept
{
	return isEnabled;
}

inline void Component::Awake()
{
	OnAwake();
}

inline void Component::Enable()
{
	OnEnable();
}

inline void Component::Start()
{
	OnStart();
}

inline void Component::Update(float deltaTime_)
{
	if (!isEnabled)
	{
		return;
	}

	if (!isStarted)
	{
		Start();
		isStarted = true;
	}

	OnUpdate(deltaTime_);
}

inline void Component::FixedUpdate(float fixedDeltaTime_)
{
	if (!isEnabled)
	{
		return;
	}

	OnFixedUpdate(fixedDeltaTime_);
}

inline void Component::LateUpdate(float deltaTime_)
{
	if (!isEnabled)
	{
		return;
	}

	OnLateUpdate(deltaTime_);
}

inline void Component::Render()
{
	if (!isEnabled)
	{
		return;
	}

	OnPreRender();
	OnRender();
	OnPostRender();
}

inline void Component::Disable()
{
	OnDisable();
}

