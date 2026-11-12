#pragma once

#include <memory>
#include <ranges>
#include <span>
#include <string_view>
#include <vector>

#include "../Core/Common.hpp"

#include "../Math/Quaternion.hpp"
#include "../Math/Vector3D.hpp"

#define TUK_FRAMEWORK_SCENE using Scene::Scene

namespace TUK::Framework
{
	class Camera;
	class GameObject;
	class GPUCommandBuffer;
	class ModelAsset;
	class Transform;

	class Scene
	{
	public:
		Scene() noexcept = default;
		virtual ~Scene() noexcept = default;

		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;

		Scene(Scene&&) = delete;
		Scene& operator=(Scene&&) = delete;

		void Enter();
		void EarlyUpdate();
		void FixedUpdate();
		void Update();
		void LateUpdate();
		void PreRender(GPUCommandBuffer& commandBuffer);
		void Render(GPUCommandBuffer& commandBuffer);
		void PostRender(GPUCommandBuffer& commandBuffer);
		void Exit();

		GameObject* Instantiate();
		GameObject* Instantiate(const Vector3D<f32>& position, const Quaternion<f32>& rotation);
		GameObject* Instantiate(const Vector3D<f32>& position);
		GameObject* Instantiate(const Quaternion<f32>& rotation);
		GameObject* Instantiate(Transform* parent);
		GameObject* Instantiate(const Vector3D<f32>& position, const Quaternion<f32>& rotation, Transform* parent);

		GameObject* Instantiate(const ModelAsset* model);
		GameObject* Instantiate(const ModelAsset* model, const Vector3D<f32>& position, const Quaternion<f32>& rotation);
		GameObject* Instantiate(const ModelAsset* model, const Vector3D<f32>& position);
		GameObject* Instantiate(const ModelAsset* model, const Quaternion<f32>& rotation);
		GameObject* Instantiate(const ModelAsset* model, Transform* parent);
		GameObject* Instantiate(const ModelAsset* model, const Vector3D<f32>& position, const Quaternion<f32>& rotation, Transform* parent);

		void Destroy(GameObject* gameObject) noexcept;

		void AddCamera(Camera* camera);
		void RemoveCamera(const Camera* camera);

		[[nodiscard]] bool IsEntered() const noexcept;

		[[nodiscard]] GameObject* FindGameObjectByName(std::string_view name) noexcept;
		[[nodiscard]] const GameObject* FindGameObjectByName(std::string_view name) const noexcept;
		[[nodiscard]] std::vector<GameObject*> FindObjectsByName(std::string_view name);
		[[nodiscard]] std::vector<const GameObject*> FindObjectsByName(std::string_view name) const;

		[[nodiscard]] GameObject* FindGameObjectByTag(std::string_view tag) noexcept;
		[[nodiscard]] const GameObject* FindGameObjectByTag(std::string_view tag) const noexcept;
		[[nodiscard]] std::vector<GameObject*> FindObjectsByTag(std::string_view tag);
		[[nodiscard]] std::vector<const GameObject*> FindObjectsByTag(std::string_view tag) const;

		[[nodiscard]] auto GetObjects() noexcept;
		[[nodiscard]] auto GetObjects() const noexcept;

		[[nodiscard]] std::span<Camera* const> GetCameras() noexcept;
		[[nodiscard]] std::span<const Camera* const> GetCameras() const noexcept;

	protected:
		virtual void OnEnter();
		virtual void OnExit();

		virtual void OnFixedUpdate();
		virtual void OnUpdate();
		virtual void OnLateUpdate();


	private:
		using SceneCallback = void (Scene::*)();
		using GameObjectCallback = void (GameObject::*)();
		using RenderCallback = void (GameObject::*)(GPUCommandBuffer&);

		template <class TCallback>
		void RunPhase(TCallback callback);

		template <class TCallback>
		void ForEachGameObject(TCallback callback);

		void UpdateObjects(SceneCallback sceneCallback, GameObjectCallback gameObjectCallback);
		void RenderObjects(RenderCallback callback, GPUCommandBuffer& commandBuffer);
		void DestroyObjects();
		void ClearObjects();

		bool isEntered = false;
		bool isRunningPhase = false;

		std::vector<std::unique_ptr<GameObject>> objects{};
		std::vector<Camera*> activeCameras{};
	};

	inline bool Scene::IsEntered() const noexcept
	{
		return isEntered;
	}

	inline auto Scene::GetObjects() noexcept
	{
		return objects | std::views::transform([](const std::unique_ptr<GameObject>& gameObjectPtr) -> GameObject*
			{
				return gameObjectPtr.get();
			});
	}

	inline auto Scene::GetObjects() const noexcept
	{
		return objects | std::views::transform([](const std::unique_ptr<GameObject>& gameObjectPtr) -> const GameObject*
			{
				return gameObjectPtr.get();
			});
	}

	inline std::span<Camera* const> Scene::GetCameras() noexcept
	{
		return activeCameras;
	}

	inline std::span<const Camera* const> Scene::GetCameras() const noexcept
	{
		return activeCameras;
	}

	inline void Scene::OnEnter()
	{
	}

	inline void Scene::OnExit()
	{
	}

	inline void Scene::OnFixedUpdate()
	{
	}

	inline void Scene::OnUpdate()
	{
	}

	inline void Scene::OnLateUpdate()
	{
	}
}
