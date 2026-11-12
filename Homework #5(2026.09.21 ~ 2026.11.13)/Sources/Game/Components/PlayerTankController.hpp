#pragma once

#include "Framework/Core.hpp"
#include "Framework/Objects.hpp"
#include "Framework/Platform.hpp"
#include "Framework/Resources.hpp"

namespace TUK::Game
{
	using namespace Framework;

	class PlayerTankController : public Component
	{
	public:
		using Component::Component;

		void OnStart() override;
		void OnUpdate() override;
		void OnLateUpdate() override;

		[[nodiscard]] f32 GetMoveSpeed() const noexcept;
		void SetMoveSpeed(f32 value) noexcept;

		[[nodiscard]] f32 GetBoostMultiplier() const noexcept;
		void SetBoostMultiplier(f32 value) noexcept;

		[[nodiscard]] f32 GetRotationSpeed() const noexcept;
		void SetRotationSpeed(f32 value) noexcept;

		[[nodiscard]] f32 GetMouseSensitivity() const noexcept;
		void SetMouseSensitivity(f32 value) noexcept;

		[[nodiscard]] f32 GetTiltAlignSpeed() const noexcept;
		void SetTiltAlignSpeed(f32 value) noexcept;

		[[nodiscard]] f32 GetProjectileSpeed() const noexcept;
		void SetProjectileSpeed(f32 value) noexcept;

		[[nodiscard]] f32 GetProjectileLifetime() const noexcept;
		void SetProjectileLifetime(f32 value) noexcept;

		[[nodiscard]] const Vector3D<f32>& GetMuzzleOffset() const noexcept;
		void SetMuzzleOffset(const Vector3D<f32>& value) noexcept;

		[[nodiscard]] i32 GetHealth() const noexcept;
		void SetHealth(i32 value) noexcept;
		[[nodiscard]] bool IsAlive() const noexcept;
		bool TakeDamage(i32 amount = 1) noexcept;

	private:
		void Fire(const Vector3D<f32>& direction);

		static constexpr i32 DEFAULT_HEALTH = 10;

		static constexpr f32 DEFAULT_MOVE_SPEED = 12.0f;
		static constexpr f32 DEFAULT_BOOST_MULTIPLIER = 2.0f;
		static constexpr f32 DEFAULT_ROTATION_SPEED = 90.0f;
		static constexpr f32 DEFAULT_MOUSE_SENSITIVITY = 0.1f;
		static constexpr f32 DEFAULT_TILT_ALIGN_SPEED = 10.0f;

		static constexpr f32 DEFAULT_PROJECTILE_SPEED = 120.0f;
		static constexpr f32 DEFAULT_PROJECTILE_LIFETIME = 5.0f;
		static inline const Vector3D<f32> DEFAULT_MUZZLE_OFFSET{ 0.0f, 2.33f, 6.43f };

		i32 health = DEFAULT_HEALTH;

		f32 moveSpeed = DEFAULT_MOVE_SPEED;
		f32 boostMultiplier = DEFAULT_BOOST_MULTIPLIER;
		f32 rotationSpeed = DEFAULT_ROTATION_SPEED;
		f32 mouseSensitivity = DEFAULT_MOUSE_SENSITIVITY;
		f32 tiltAlignSpeed = DEFAULT_TILT_ALIGN_SPEED;

		Quaternion<f32> yawRotation = Quaternion<f32>::GetIdentity();
		Vector3D<f32> smoothedUp = Vector3D<f32>::GetUp();

		f32 projectileSpeed = DEFAULT_PROJECTILE_SPEED;
		f32 projectileLifetime = DEFAULT_PROJECTILE_LIFETIME;
		Vector3D<f32> muzzleOffset = DEFAULT_MUZZLE_OFFSET;

		const MeshAsset* projectileMesh = nullptr;
		const MaterialAsset* projectileMaterial = nullptr;
	};

	inline f32 PlayerTankController::GetMoveSpeed() const noexcept
	{
		return moveSpeed;
	}

	inline void PlayerTankController::SetMoveSpeed(f32 value) noexcept
	{
		moveSpeed = value;
	}

	inline f32 PlayerTankController::GetBoostMultiplier() const noexcept
	{
		return boostMultiplier;
	}

	inline void PlayerTankController::SetBoostMultiplier(f32 value) noexcept
	{
		boostMultiplier = value;
	}

	inline f32 PlayerTankController::GetRotationSpeed() const noexcept
	{
		return rotationSpeed;
	}

	inline void PlayerTankController::SetRotationSpeed(f32 value) noexcept
	{
		rotationSpeed = value;
	}

	inline f32 PlayerTankController::GetMouseSensitivity() const noexcept
	{
		return mouseSensitivity;
	}

	inline void PlayerTankController::SetMouseSensitivity(f32 value) noexcept
	{
		mouseSensitivity = value;
	}

	inline f32 PlayerTankController::GetTiltAlignSpeed() const noexcept
	{
		return tiltAlignSpeed;
	}

	inline void PlayerTankController::SetTiltAlignSpeed(f32 value) noexcept
	{
		tiltAlignSpeed = value;
	}

	inline f32 PlayerTankController::GetProjectileSpeed() const noexcept
	{
		return projectileSpeed;
	}

	inline void PlayerTankController::SetProjectileSpeed(f32 value) noexcept
	{
		projectileSpeed = value;
	}

	inline f32 PlayerTankController::GetProjectileLifetime() const noexcept
	{
		return projectileLifetime;
	}

	inline void PlayerTankController::SetProjectileLifetime(f32 value) noexcept
	{
		projectileLifetime = value;
	}

	inline const Vector3D<f32>& PlayerTankController::GetMuzzleOffset() const noexcept
	{
		return muzzleOffset;
	}

	inline void PlayerTankController::SetMuzzleOffset(const Vector3D<f32>& value) noexcept
	{
		muzzleOffset = value;
	}

	inline i32 PlayerTankController::GetHealth() const noexcept
	{
		return health;
	}

	inline void PlayerTankController::SetHealth(i32 value) noexcept
	{
		health = std::max(0, value);
	}

	inline bool PlayerTankController::IsAlive() const noexcept
	{
		return health > 0;
	}

	inline bool PlayerTankController::TakeDamage(i32 amount) noexcept
	{
		health = std::max(0, health - amount);
		return health <= 0;
	}
}
