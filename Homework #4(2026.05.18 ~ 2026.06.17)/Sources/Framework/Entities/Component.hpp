#pragma once

namespace TUK::Framework
{
	class Entity;
	class D3DCommandBuffer;
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

		virtual void OnPreRender(D3DCommandBuffer& renderer);
		virtual void OnRender(D3DCommandBuffer& renderer);
		virtual void OnPostRender(D3DCommandBuffer& renderer);

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

	inline void Component::OnPreRender(D3DCommandBuffer&)
	{
	}

	inline void Component::OnRender(D3DCommandBuffer&)
	{
	}

	inline void Component::OnPostRender(D3DCommandBuffer&)
	{
	}

	inline void Component::OnDestroy()
	{
	}
}
