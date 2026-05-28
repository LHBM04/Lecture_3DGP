#pragma once

class GameObject;

class Component
{
public:
	Component() noexcept = default;
	~Component() = default;

	Component(const Component&) = delete;
	Component& operator=(const Component&) = delete;

	Component(Component&&) = delete;
	Component& operator=(Component&&) = delete;

	void NotifyAdd(GameObject* owner_);
	void NotifyRemove();

	[[nodiscard]] bool IsAttached() const;

	[[nodiscard]] bool IsEnabled() const;
	void SetEnabled(bool enabled_);

	[[nodiscard]] bool IsDestroyed() const;
	void Destroy();

	[[nodiscard]] GameObject* GetOwner();
	[[nodiscard]] const GameObject* GetOwner() const;

protected:
	virtual void OnAwake() {}
	virtual void OnEnable() {}
	virtual void OnStart() {}
	virtual void OnDisable() {}
	virtual void OnDestroy() {}

private:
	bool isAwakened{ false };
	bool isEnabled{ true };
	bool isStarted{ false };
	bool isDestroyed{ false };

	GameObject* owner{ nullptr };
};
