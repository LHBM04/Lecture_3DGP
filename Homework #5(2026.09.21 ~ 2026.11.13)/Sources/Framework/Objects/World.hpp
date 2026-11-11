#pragma once

#include <memory>
#include <ranges>
#include <span>
#include <string_view>
#include <vector>

#include "../Core/Common.hpp"

#include "../Math/Quaternion.hpp"
#include "../Math/Vector3D.hpp"

#define TUK_FRAMEWORK_WORLD using World::World

namespace TUK::Framework
{
	class Camera;
	class GameObject;
	class GPUCommandBuffer;
	class MaterialAsset;
	class ModelAsset;
	class Transform;

	class World
	{
	public:
		World() noexcept = default;
		virtual ~World() noexcept = default;

		World(const World&) = delete;
		World& operator=(const World&) = delete;

		World(World&&) = delete;
		World& operator=(World&&) = delete;

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

		void SetSkyboxMaterial(const MaterialAsset* value) noexcept;

	private:
		using WorldCallback = void (World::*)();
		using GameObjectCallback = void (GameObject::*)();
		using RenderCallback = void (GameObject::*)(GPUCommandBuffer&);

		template <class TCallback>
		void RunPhase(TCallback callback);

		template <class TCallback>
		void ForEachGameObject(TCallback callback);

		void UpdateObjects(WorldCallback worldCallback, GameObjectCallback gameObjectCallback);
		void RenderObjects(RenderCallback callback, GPUCommandBuffer& commandBuffer);
		void DestroyObjects();
		void ClearObjects();

		bool isEntered = false;
		bool isRunningPhase = false;

		std::vector<std::unique_ptr<GameObject>> objects{};
		std::vector<Camera*> cameras{};
		const MaterialAsset* skyboxMaterial = nullptr;
	};

	inline void World::SetSkyboxMaterial(const MaterialAsset* value) noexcept
	{
		skyboxMaterial = value;
	}

	inline bool World::IsEntered() const noexcept
	{
		return isEntered;
	}

	inline auto World::GetObjects() noexcept
	{
		return objects | std::views::transform([](const std::unique_ptr<GameObject>& gameObjectPtr) -> GameObject*
			{
				return gameObjectPtr.get();
			});
	}

	inline auto World::GetObjects() const noexcept
	{
		return objects | std::views::transform([](const std::unique_ptr<GameObject>& gameObjectPtr) -> const GameObject*
			{
				return gameObjectPtr.get();
			});
	}

	inline std::span<Camera* const> World::GetCameras() noexcept
	{
		return cameras;
	}

	inline std::span<const Camera* const> World::GetCameras() const noexcept
	{
		return cameras;
	}

	inline void World::OnEnter()
	{
	}

	inline void World::OnExit()
	{
	}

	inline void World::OnFixedUpdate()
	{
	}

	inline void World::OnUpdate()
	{
	}

	inline void World::OnLateUpdate()
	{
	}
}
