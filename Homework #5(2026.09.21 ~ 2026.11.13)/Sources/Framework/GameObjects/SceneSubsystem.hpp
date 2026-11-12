#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "../Core/AppSubsystem.hpp"
#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class GPUCommandBuffer;
	class Scene;

	class SceneSubsystem : public AppSubsystem
	{
	public:
		SceneSubsystem() noexcept;
		~SceneSubsystem() noexcept override;

		Scene* AddScene(std::unique_ptr<Scene> scene);
		void ExitScene();

		[[nodiscard]] bool RequestSceneChange(Scene* targetScene) noexcept;

		[[nodiscard]] Delegate<Scene*>& OnEnterScene() noexcept;
		[[nodiscard]] Delegate<Scene*>& OnExitScene() noexcept;

		[[nodiscard]] Scene* GetCurrentScene() noexcept;
		[[nodiscard]] const Scene* GetCurrentScene() const noexcept;

		void PreRender(GPUCommandBuffer& commandBuffer);
		void Render(GPUCommandBuffer& commandBuffer);
		void PostRender(GPUCommandBuffer& commandBuffer);

		SystemStatus OnStartup() override;
		SystemStatus OnEarlyUpdate() override;
		SystemStatus OnFixedUpdate() override;
		SystemStatus OnUpdate() override;
		SystemStatus OnLateUpdate() override;
		void OnShutdown() override;

	private:
		void ExitCurrentScene();
		void NotifyIfSceneExited(Scene* scene);

		std::vector<std::unique_ptr<Scene>> scenes;
		Scene* currentScene = nullptr;
		Scene* nextScene = nullptr;

		Delegate<Scene*> onEnterScene;
		Delegate<Scene*> onExitScene;
	};

	inline SceneSubsystem::SceneSubsystem() noexcept
		: AppSubsystem(20)
		, scenes()
	{
	}

	inline Delegate<Scene*>& SceneSubsystem::OnEnterScene() noexcept
	{
		return onEnterScene;
	}

	inline Delegate<Scene*>& SceneSubsystem::OnExitScene() noexcept
	{
		return onExitScene;
	}

	inline Scene* SceneSubsystem::GetCurrentScene() noexcept
	{
		return currentScene;
	}

	inline const Scene* SceneSubsystem::GetCurrentScene() const noexcept
	{
		return currentScene;
	}
}
