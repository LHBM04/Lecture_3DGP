#pragma once

namespace TUK::Framework
{
	class Entity;
	class GPUCommandBuffer;
	class World;

	class Component
	{
		friend class Entity;

	public:
		Component(Entity& owner) noexcept;
		virtual ~Component() noexcept = default;

		/** 복사 금지 */
		Component(const Component&) = delete;
		Component& operator=(const Component&) = delete;

		/** 이동 금지 */
		Component(Component&&) = default;
		Component& operator=(Component&&) = default;

		[[nodiscard]] const Entity& GetOwner() const;
		[[nodiscard]] Entity& GetOwner();

	protected:
		virtual void OnAwake();
		virtual void OnStart();

		virtual void OnEnable();
		virtual void OnDisable();

		virtual void OnFixedUpdate();
		virtual void OnUpdate();
		virtual void OnLateUpdate();

		virtual void OnPreRender(GPUCommandBuffer& renderer);
		virtual void OnRender(GPUCommandBuffer& renderer);
		virtual void OnPostRender(GPUCommandBuffer& renderer);

		virtual void OnDestroy();

	private:
		Entity& owner;

		bool isStared;
		bool isEnabled;
		bool isDestroyed;
	};

	inline Component::Component(Entity& owner) noexcept
		: owner(owner)
		, isStared(false)
		, isEnabled(true)
		, isDestroyed(false)
	{
	}

	inline const Entity& Component::GetOwner() const
	{
		return owner;
	}

	inline Entity& Component::GetOwner()
	{
		return owner;
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
}
