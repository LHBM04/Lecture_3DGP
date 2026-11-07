#pragma once

#include "../Core/Common.hpp"

#include "../Utilities/Logger.hpp"

namespace TUK::Framework
{
	class Entity;
	class GPUCommandBuffer;

	class Component
	{
	public:
		explicit Component(Entity* owner) noexcept;
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

		virtual void OnDestroy();

		[[nodiscard]] const Entity* GetOwner() const noexcept;
		[[nodiscard]] Entity* GetOwner() noexcept;

		[[nodiscard]] bool IsStarted() const noexcept;
		void SetStarted(bool value) noexcept;

		[[nodiscard]] bool IsEnabled() const noexcept;
		void SetEnabled(bool value) noexcept;

		[[nodiscard]] bool IsDestroyed() const noexcept;
		void Destroy() noexcept;

	private:
		Entity* owner = nullptr;

		bool isStarted = false;
		bool isEnabled = true;
		bool isDestroyed = false;
	};

	inline Component::Component(Entity* owner) noexcept
		: owner(owner)
	{
		ASSERT_FMT(owner, "而댄룷?뚰듃 ?뚯쑀?먭? nullptr?낅땲??");
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

	inline void Component::OnDestroy()
	{
	}

	inline const Entity* Component::GetOwner() const noexcept
	{
		return owner;
	}

	inline Entity* Component::GetOwner() noexcept
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
