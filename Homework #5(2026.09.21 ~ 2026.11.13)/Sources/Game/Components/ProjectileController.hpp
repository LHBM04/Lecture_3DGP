#pragma once

#include <string>
#include <string_view>

#include "Framework/Objects.hpp"

namespace TUK::Game
{
	using namespace Framework;

	class ProjectileController final : public Component
	{
	public:
		using Component::Component;

		void OnStart() override;
		void OnUpdate() override;

		[[nodiscard]] const Vector3D<f32>& GetLaunchPosition() const noexcept;
		void SetLaunchPosition(const Vector3D<f32>& value) noexcept;

		[[nodiscard]] const Vector3D<f32>& GetLaunchDirection() const noexcept;
		void SetLaunchDirection(const Vector3D<f32>& value) noexcept;

		[[nodiscard]] f32 GetMoveSpeed() const noexcept;
		void SetMoveSpeed(f32 value) noexcept;

		[[nodiscard]] f32 GetLifetime() const noexcept;
		void SetLifetime(f32 value) noexcept;

		[[nodiscard]] std::string_view GetTargetTag() const noexcept;
		void SetTargetTag(std::string_view value);

		[[nodiscard]] f32 GetElapsedTime() const noexcept;
		void SetElapsedTime(f32 value) noexcept;

	private:
		static void DestroyHierarchy(GameObject* gameObject) noexcept;

		Vector3D<f32> launchPosition = Vector3D<f32>::GetZero();
		Vector3D<f32> launchDirection = Vector3D<f32>::GetForward();
		f32 moveSpeed = 0.0f;
		f32 lifetime = 0.0f;
		std::string targetTag;

		f32 elapsedTime = 0.0f;
	};

	inline const Vector3D<f32>& ProjectileController::GetLaunchPosition() const noexcept
	{
		return launchPosition;
	}

	inline void ProjectileController::SetLaunchPosition(const Vector3D<f32>& value) noexcept
	{
		launchPosition = value;
		if (GameObject* owner = GetOwner())
		{
			if (Transform* transform = owner->GetComponent<Transform>())
			{
				transform->SetWorldPosition(value);
			}
		}
	}

	inline const Vector3D<f32>& ProjectileController::GetLaunchDirection() const noexcept
	{
		return launchDirection;
	}

	inline void ProjectileController::SetLaunchDirection(const Vector3D<f32>& value) noexcept
	{
		launchDirection = value.IsZero() ? Vector3D<f32>::GetForward() : value.GetNormalized();
	}

	inline f32 ProjectileController::GetMoveSpeed() const noexcept
	{
		return moveSpeed;
	}

	inline void ProjectileController::SetMoveSpeed(f32 value) noexcept
	{
		moveSpeed = value;
	}

	inline f32 ProjectileController::GetLifetime() const noexcept
	{
		return lifetime;
	}

	inline void ProjectileController::SetLifetime(f32 value) noexcept
	{
		lifetime = value;
	}

	inline std::string_view ProjectileController::GetTargetTag() const noexcept
	{
		return targetTag;
	}

	inline void ProjectileController::SetTargetTag(std::string_view value)
	{
		targetTag = value;
	}

	inline f32 ProjectileController::GetElapsedTime() const noexcept
	{
		return elapsedTime;
	}

	inline void ProjectileController::SetElapsedTime(f32 value) noexcept
	{
		elapsedTime = value;
	}
}
