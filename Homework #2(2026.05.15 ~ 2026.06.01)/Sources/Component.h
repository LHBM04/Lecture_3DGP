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

	void NotifyAttach(GameObject* owner_);
	void NotifyDetach();

protected:
	virtual void OnAttach() {}
	virtual void OnUpdate() {}
	virtual void OnRender() {}
	virtual void OnDetach() {}

private:
	bool isEnabled{ true };
	bool isDestroyed{ false };

	GameObject* owner{ nullptr };
};

