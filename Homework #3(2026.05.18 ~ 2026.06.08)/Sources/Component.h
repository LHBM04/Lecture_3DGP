#pragma once

class Collider;
class GameObject;

class Component
{
	friend class GameObject;

public:
	Component() noexcept = default;
	virtual ~Component() noexcept = default;

	[[nodiscard]] GameObject* GetOwner() const noexcept;
	void SetOwner(GameObject* owner_) noexcept;

	[[nodiscard]] bool IsStarted() const noexcept;
	[[nodiscard]] bool IsActive() const noexcept;

	[[nodiscard]] bool IsEnabled() const noexcept;
	void SetEnabled(bool isEnabled_);

	void Awake();
	void Enable();
	void Start();

	void Update();
	void FixedUpdate();
	void LateUpdate();

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

	virtual void OnUpdate() {}
	virtual void OnFixedUpdate() {}
	virtual void OnLateUpdate() {}

	virtual void OnPreRender() {}
	virtual void OnRender() {}
	virtual void OnPostRender() {}

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
	bool isStarted{ false };
	bool isEnabled{ true };
	bool isDestroyed{ false };
};
