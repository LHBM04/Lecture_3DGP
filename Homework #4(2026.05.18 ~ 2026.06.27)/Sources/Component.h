#pragma once

class Collider;
class GameObject;
class RenderContext;

class Component
{
	friend class GameObject;

public:
	Component() noexcept = default;
	virtual ~Component() noexcept;

	[[nodiscard]] GameObject& GetOwner() noexcept;
	[[nodiscard]] const GameObject& GetOwner() const noexcept;

	[[nodiscard]] bool IsStarted() const noexcept;
	[[nodiscard]] bool IsEnabled() const noexcept;
	[[nodiscard]] bool IsActive() const noexcept;
	void SetEnabled(bool isEnabled_);

	void Awake();
	void Enable();
	void Start();
	void Update();
	void LateUpdate();
	void FixedUpdate();
	void Render(const RenderContext& context_);
	void Disable();
	void Destroy();

protected:
	virtual void OnAwake() {}
	virtual void OnEnable() {}
	virtual void OnStart() {}

	virtual void OnUpdate() {}
	virtual void OnLateUpdate() {}
	virtual void OnFixedUpdate() {}

	virtual void OnPreRender(const RenderContext& context_) {}
	virtual void OnRender(const RenderContext& context_) {}
	virtual void OnPostRender(const RenderContext& context_) {}

	virtual void OnDisable() {}
	virtual void OnDestroy() {}

	virtual void OnCollisionEnter(Collider& other_) {}
	virtual void OnCollisionStay(Collider& other_) {}
	virtual void OnCollisionExit(Collider& other_) {}

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
