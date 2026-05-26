#pragma once

class GameObject;

class Component
{
	friend class GameObject;

public:
	Component() noexcept = default;
	virtual ~Component() = default;

	Component(const Component&) = delete;
	Component& operator=(const Component&) = delete;

	Component(Component&&) = delete;
	Component& operator=(Component&&) = delete;

	[[nodiscard]] bool IsAttached() const;

	[[nodiscard]] bool IsEnabled() const;
	void SetEnabled(bool enabled_);

	[[nodiscard]] bool IsDestroyed() const;
	void Destroy();

	[[nodiscard]] GameObject* GetOwner();
	[[nodiscard]] const GameObject* GetOwner() const;
	[[nodiscard]] virtual bool WantsCollisionEvents() const noexcept { return false; }

	void NotifyAttach(GameObject* owner_);
	void NotifyDetach();

protected:
	virtual void OnReset() {}
	virtual void OnAwake() {}
	virtual void OnEnable() {}
	virtual void OnStart() {}
	virtual void OnDisable() {}
	virtual void OnDestroy() {}

	// Legacy hooks. Keep for compatibility with existing components.
	virtual void OnAttach() {}
	virtual void OnUpdate() {}
	virtual void OnFixedUpdate() {}
	virtual void OnRender() {}
	virtual void OnCollisionEnter(GameObject& other_) {}
	virtual void OnCollisionStay(GameObject& other_) {}
	virtual void OnCollisionExit(GameObject& other_) {}
	virtual void OnDetach() {}

private:
	void TickUpdate();
	void TickFixedUpdate();
	void TickRender();
	void TryAwake();
	void TryEnable();
	void TryDisable();
	void TryDestroy();

	bool isEnabled{ true };
	bool isDestroyed{ false };
	bool isAwakened{ false };
	bool isStarted{ false };
	bool isActiveAndEnabled{ false };
	bool isDestroyNotified{ false };

	GameObject* owner{ nullptr };
};
