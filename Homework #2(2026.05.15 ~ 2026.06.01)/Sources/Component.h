#pragma once

class GameObject;

template <class TDerived>
class Component
{
public:
	Component() = default;
	~Component() = default;

	Component(const Component&) = delete;
	Component& operator=(const Component&) = delete;
	
	Component(Component&&) = delete;
	Component& operator=(Component&&) = delete;

	[[nodiscard]] GameObject* GetOwner() const noexcept;
	void SetOwner(GameObject* owner_) noexcept;

	[[nodiscard]] bool IsStarted() const noexcept;
	
	[[nodiscard]] bool IsActive() const noexcept;
	[[nodiscard]] bool IsEnabled() const noexcept;
	void SetEnabled(bool isEnabled_) noexcept;

	void Awake() noexcept;
	void Enable() noexcept;
	void Start() noexcept;
	
	void Update(float deltaTime_) noexcept;
	void FixedUpdate(float fixedDeltaTime_) noexcept;
	void LateUpdate(float deltaTime_) noexcept;
	
	void Render() noexcept;
	
	void Disable() noexcept;
	void Destroy() noexcept;

private:
	GameObject* owner{ nullptr };
	bool isStarted{ false };
	bool isEnabled{ true };
};

// ============================================================================
// Inline Implementations
// ============================================================================

template <class TDerived>
inline GameObject* Component<TDerived>::GetOwner() const noexcept
{
	return owner;
}

template <class TDerived>
inline void Component<TDerived>::SetOwner(GameObject* owner_) noexcept
{
	owner = owner_;
}

template <class TDerived>
inline bool Component<TDerived>::IsStarted() const noexcept
{
	return isStarted;
}

template <class TDerived>
inline bool Component<TDerived>::IsEnabled() const noexcept
{
	return isEnabled;
}

template <class TDerived>
inline void Component<TDerived>::Awake() noexcept
{
	if constexpr (requires(TDerived d) { d.OnAwake(); })
	{
		static_cast<TDerived*>(this)->OnAwake();
	}
}

template <class TDerived>
inline void Component<TDerived>::Enable() noexcept
{
	if constexpr (requires(TDerived d) { d.OnEnable(); })
	{
		static_cast<TDerived*>(this)->OnEnable();
	}
}

template <class TDerived>
inline void Component<TDerived>::Start() noexcept
{
	if constexpr (requires(TDerived d) { d.OnStart(); })
	{
		static_cast<TDerived*>(this)->OnStart();
	}
}

template <class TDerived>
inline void Component<TDerived>::Update(float deltaTime_) noexcept
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

	if constexpr (requires(TDerived d, float dt) { d.OnUpdate(dt); })
	{
		static_cast<TDerived*>(this)->OnUpdate(deltaTime_);
	}
}

template <class TDerived>
inline void Component<TDerived>::FixedUpdate(float fixedDeltaTime_) noexcept
{
	if (!isEnabled)
	{
		return;
	}

	if constexpr (requires(TDerived d, float dt) { d.OnFixedUpdate(dt); })
	{
		static_cast<TDerived*>(this)->OnFixedUpdate(fixedDeltaTime_);
	}
}

template <class TDerived>
inline void Component<TDerived>::LateUpdate(float deltaTime_) noexcept
{
	if (!isEnabled)
	{
		return;
	}

	if constexpr (requires(TDerived d, float dt) { d.OnLateUpdate(dt); })
	{
		static_cast<TDerived*>(this)->OnLateUpdate(deltaTime_);
	}
}

template <class TDerived>
inline void Component<TDerived>::Render() noexcept
{
	if (!isEnabled)
	{
		return;
	}

	if constexpr (requires(TDerived d) { d.OnPreRender(); })
	{
		static_cast<TDerived*>(this)->OnPreRender();
	}

	if constexpr (requires(TDerived d) { d.OnRender(); })
	{
		static_cast<TDerived*>(this)->OnRender();
	}

	if constexpr (requires(TDerived d) { d.OnPostRender(); })
	{
		static_cast<TDerived*>(this)->OnPostRender();
	}
}

template <class TDerived>
inline void Component<TDerived>::Disable() noexcept
{
	if constexpr (requires(TDerived d) { d.OnDisable(); })
	{
		static_cast<TDerived*>(this)->OnDisable();
	}
}

template <class TDerived>
inline void Component<TDerived>::Destroy() noexcept
{
	if constexpr (requires(TDerived d) { d.OnDestroy(); })
	{
		static_cast<TDerived*>(this)->OnDestroy();
	}
}

#include "GameObject.h"

template <class TDerived>
inline bool Component<TDerived>::IsActive() const noexcept
{
	return isEnabled && owner && owner->IsActive();
}

template <class TDerived>
inline void Component<TDerived>::SetEnabled(bool isEnabled_) noexcept
{
	if (isEnabled == isEnabled_)
	{
		return;
	}
	isEnabled = isEnabled_;

	if (owner && owner->IsActive())
	{
		if (isEnabled)
		{
			Enable();
		}
		else
		{
			Disable();
		}
	}
}
