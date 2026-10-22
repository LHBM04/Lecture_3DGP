#pragma once

#include <memory>
#include <vector>

namespace TUK::Framework
{
	class Entity;
	class CommandBuffer;

	class World
	{
	public:
		World() noexcept;
		virtual ~World() noexcept;

		/** 복사 금지 */
		World(const World&) = delete;
		World& operator=(const World&) = delete;
		
		/** 이동 금지 */
		World(World&&) = delete;
		World& operator=(World&&) = delete;

		void Enter();
		void FixedUpdate();
		void Update();
		void LateUpdate();
		void PreRender(CommandBuffer& renderer);
		void Render(CommandBuffer& renderer);
		void PostRender(CommandBuffer& renderer);
		void Exit();

		Entity& CreateEntity();

	protected:
		void ClearEntities() noexcept;
		virtual void OnEnter();
		virtual void OnFixedUpdate();
		virtual void OnUpdate();
		virtual void OnLateUpdate();
		virtual void OnPreRender(CommandBuffer& renderer);
		virtual void OnRender(CommandBuffer& renderer);
		virtual void OnPostRender(CommandBuffer& renderer);
		virtual void OnExit();

	private:
		std::vector<std::unique_ptr<Entity>> entities;
		bool isEntered;
	};
}
