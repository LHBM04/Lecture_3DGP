#pragma once

namespace TUK::Framework
{
	class Entity;
	class CommandBuffer;

	class Component
	{
		friend class Entity;

	public:
		Component() noexcept;
		virtual ~Component() noexcept;

		/** 복사 금지 */
		Component(const Component&) = delete;
		Component& operator=(const Component&) = delete;

		/** 이동 금지 */
		Component(Component&&) = delete;
		Component& operator=(Component&&) = delete;

		/** 소유자 가져오기 */
		[[nodiscard]] const Entity* GetOwner() const;
		[[nodiscard]] Entity* GetOwner();

	protected:
		virtual void OnAwake();
		virtual void OnStart();

		virtual void OnEnable();
		virtual void OnDisable();

		virtual void OnFixedUpdate();
		virtual void OnUpdate();
		virtual void OnLateUpdate();

		virtual void OnPreRender(CommandBuffer& renderer);
		virtual void OnRender(CommandBuffer& renderer);
		virtual void OnPostRender(CommandBuffer& renderer);

		virtual void OnDestroy();

	private:
		Entity* owner;

		bool isStared;
		bool isEnabled;
		bool isDestroyed;
	};
}
