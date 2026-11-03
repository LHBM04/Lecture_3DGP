#pragma once

#include <memory>
#include <string_view>
#include <vector>

namespace TUK::Framework
{
	class Camera;
	class Entity;
	class GPUCommandBuffer;

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
		void PreRender(GPUCommandBuffer& commandBuffer);
		void Render(GPUCommandBuffer& commandBuffer);
		void PostRender(GPUCommandBuffer& commandBuffer);
		void Exit();

		Entity& CreateEntity();
		Entity& CreateEntity(std::string_view name, std::string_view tag);

		void AddCamera(Camera& camera);
		void RemoveCamera(const Camera& camera);
		// 등록된 첫 카메라를 사용합니다. 등록된 카메라가 없으면 nullptr입니다.
		[[nodiscard]] Camera* GetMainCamera() noexcept;
		[[nodiscard]] const Camera* GetMainCamera() const noexcept;

	protected:
		virtual void OnEnter();
		virtual void OnFixedUpdate();
		virtual void OnUpdate();
		virtual void OnLateUpdate();
		virtual void OnPreRender(GPUCommandBuffer& commandBuffer);
		virtual void OnRender(GPUCommandBuffer& commandBuffer);
		virtual void OnPostRender(GPUCommandBuffer& commandBuffer);
		virtual void OnExit();

	private:
		std::vector<Camera*> cameras;
		std::vector<std::unique_ptr<Entity>> entities;
		bool isEntered;
	};
}
