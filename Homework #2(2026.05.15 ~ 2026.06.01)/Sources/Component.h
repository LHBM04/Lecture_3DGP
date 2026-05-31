#pragma once

class Collider;
class GameObject;

class Component
{
public:
	Component() = default;
	virtual ~Component();

	Component(const Component&) = delete;
	Component& operator=(const Component&) = delete;
	
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

	void CollisionEnter(Collider* other_);
	void CollisionStay(Collider* other_);
	void CollisionExit(Collider* other_);

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

	virtual void OnCollisionEnter([[maybe_unused]] Collider* other_) {}
	virtual void OnCollisionStay([[maybe_unused]] Collider* other_) {}
	virtual void OnCollisionExit([[maybe_unused]] Collider* other_) {}

private:
	GameObject* owner{ nullptr };
	bool isStarted{ false };
	bool isEnabled{ true };
	bool isDestroyed{ false };
};
