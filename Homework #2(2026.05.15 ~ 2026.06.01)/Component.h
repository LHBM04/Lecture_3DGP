#pragma once

class Transform;
class GameObject;
class RenderTarget;

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

	[[nodiscard]] inline bool IsAttached() const;

	[[nodiscard]] inline bool IsEnabled() const;
	inline void SetEnabled(bool enabled_);

	[[nodiscard]] inline bool IsDestroyed() const;
	inline void Destroy();

	[[nodiscard]] inline GameObject* GetOwner();
	[[nodiscard]] inline const GameObject* GetOwner() const;

	void NotifyAttach(GameObject* const owner_);
	void NotifyDetach();

protected:
	virtual void OnAttach() {}
	virtual void OnUpdate() {}
	virtual void OnRender(RenderTarget&) {}
	virtual void OnDetach() {}

private:
	bool isEnabled{ true };
	bool isDestroyed{ false };

	GameObject* owner{ nullptr };
};

inline bool Component::IsAttached() const
{
	return nullptr != owner;
}

inline bool Component::IsEnabled() const
{
	return isEnabled;
}

inline void Component::SetEnabled(bool enabled_)
{
	isEnabled = enabled_;
}

inline bool Component::IsDestroyed() const
{
	return isDestroyed;
}

inline void Component::Destroy()
{
	isDestroyed = true;
}

GameObject* Component::GetOwner()
{
	return owner;
}

const GameObject* Component::GetOwner() const
{
	return owner;
}
