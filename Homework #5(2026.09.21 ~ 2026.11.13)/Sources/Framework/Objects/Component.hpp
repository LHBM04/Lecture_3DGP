#pragma once

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class GameObject;
	class GPUCommandBuffer;

	class Component
	{
	public:
		explicit Component(GameObject* owner) noexcept;
		virtual ~Component() noexcept = default;

		Component(const Component&) = delete;
		Component& operator=(const Component&) = delete;

		Component(Component&&) noexcept = default;
		Component& operator=(Component&&) noexcept = default;

		virtual void OnAwake();
		virtual void OnStart();

		virtual void OnEnable();
		virtual void OnDisable();

		virtual void OnFixedUpdate();
		virtual void OnUpdate();
		virtual void OnLateUpdate();

		virtual void OnPreRender(GPUCommandBuffer& commandBuffer);
		virtual void OnRender(GPUCommandBuffer& commandBuffer);
		virtual void OnPostRender(GPUCommandBuffer& commandBuffer);
#ifdef _DEBUG
		virtual void OnDrawGizmos(GPUCommandBuffer& commandBuffer);
#endif

		virtual void OnDestroy();

		[[nodiscard]] const GameObject* GetOwner() const noexcept;
		[[nodiscard]] GameObject* GetOwner() noexcept;

		[[nodiscard]] bool IsStarted() const noexcept;
		void SetStarted(bool value) noexcept;

		[[nodiscard]] bool IsEnabled() const noexcept;
		void SetEnabled(bool value) noexcept;

		[[nodiscard]] bool IsDestroyed() const noexcept;
		void Destroy() noexcept;

	private:
		GameObject* owner = nullptr;

		bool isStarted = false;
		bool isEnabled = true;
		bool isDestroyed = false;
	};

	inline Component::Component(GameObject* owner) noexcept
		: owner(owner)
	{
		ASSERT_FMT(owner, "컴포넌트 소유자가 nullptr입니다.");
	}

	inline void Component::OnAwake()
	{
	}

	inline void Component::OnStart()
	{
	}

	inline void Component::OnEnable()
	{
	}

	inline void Component::OnDisable()
	{
	}

	inline void Component::OnFixedUpdate()
	{
	}

	inline void Component::OnUpdate()
	{
	}

	inline void Component::OnLateUpdate()
	{
	}

	inline void Component::OnPreRender(GPUCommandBuffer&)
	{
	}

	inline void Component::OnRender(GPUCommandBuffer&)
	{
	}

	inline void Component::OnPostRender(GPUCommandBuffer&)
	{
	}

#ifdef _DEBUG
	inline void Component::OnDrawGizmos(GPUCommandBuffer&)
	{
	}
#endif

	inline void Component::OnDestroy()
	{
	}

	inline const GameObject* Component::GetOwner() const noexcept
	{
		return owner;
	}

	inline GameObject* Component::GetOwner() noexcept
	{
		return owner;
	}

	inline bool Component::IsStarted() const noexcept
	{
		return isStarted;
	}

	inline void Component::SetStarted(bool value) noexcept
	{
		isStarted = value;
	}

	inline bool Component::IsEnabled() const noexcept
	{
		return isEnabled;
	}

	inline void Component::SetEnabled(bool value) noexcept
	{
		if (isDestroyed || isEnabled == value)
		{
			return;
		}

		isEnabled = value;
		if (isEnabled)
		{
			OnEnable();
		}
		else
		{
			OnDisable();
		}
	}

	inline bool Component::IsDestroyed() const noexcept
	{
		return isDestroyed;
	}

	inline void Component::Destroy() noexcept
	{
		isDestroyed = true;
	}
}
