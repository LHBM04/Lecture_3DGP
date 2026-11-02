#pragma once

#include <memory>
#include <memory_resource>
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
	class Entity;
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

		virtual void OnEnter();
		virtual void OnExit();

		virtual void OnFixedUpdate();
		virtual void OnUpdate();
		virtual void OnLateUpdate();

		[[nodiscard]] bool IsEntered() const noexcept;
		void SetEntered(bool value) noexcept;

		Entity& Instantiate();
		Entity& Instantiate(std::string_view name);
		Entity& Instantiate(std::string_view name, std::string_view tag);
		Entity& Instantiate(const Vector3D<f32>& position, const Quaternion<f32>& rotation);
		Entity& Instantiate(const Vector3D<f32>& position);
		Entity& Instantiate(const Quaternion<f32>& rotation);
		Entity& Instantiate(Transform* parent);
		Entity& Instantiate(const Vector3D<f32>& position, const Quaternion<f32>& rotation, Transform* parent);

		void Destroy(Entity& entity) noexcept;
		void Destroy(Entity* entity) noexcept;

		void DestroyEntities();
		void ClearEntities();

		void AddCamera(Camera& camera);
		void RemoveCamera(const Camera& camera);

		[[nodiscard]] Entity* FindEntityWithName(std::string_view name) noexcept;
		[[nodiscard]] const Entity* FindEntityWithName(std::string_view name) const noexcept;
		[[nodiscard]] std::vector<Entity*> FindEntitiesWithName(std::string_view name);
		[[nodiscard]] std::vector<const Entity*> FindEntitiesWithName(std::string_view name) const;

		[[nodiscard]] Entity* FindEntityWithTag(std::string_view tag) noexcept;
		[[nodiscard]] const Entity* FindEntityWithTag(std::string_view tag) const noexcept;
		[[nodiscard]] std::vector<Entity*> FindEntitiesWithTag(std::string_view tag);
		[[nodiscard]] std::vector<const Entity*> FindEntitiesWithTag(std::string_view tag) const;

		[[nodiscard]] std::span<const std::unique_ptr<Entity>> GetEntities() const noexcept;

		[[nodiscard]] std::span<Camera* const> GetCameras() noexcept;
		[[nodiscard]] std::span<const Camera* const> GetCameras() const noexcept;

	private:
		bool isEntered = false;

		std::pmr::vector<std::unique_ptr<Entity>> entities{};
		std::pmr::vector<Camera*> cameras{};
	};

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

	inline bool World::IsEntered() const noexcept
	{
		return isEntered;
	}

	inline void World::SetEntered(bool value) noexcept
	{
		isEntered = value;
	}

	inline std::span<const std::unique_ptr<Entity>> World::GetEntities() const noexcept
	{
		return entities;
	}

	inline std::span<Camera* const> World::GetCameras() noexcept
	{
		return cameras;
	}

	inline std::span<const Camera* const> World::GetCameras() const noexcept
	{
		return cameras;
	}
}
