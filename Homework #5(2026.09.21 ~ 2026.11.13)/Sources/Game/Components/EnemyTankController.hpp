#pragma once

#include "Framework/Core.hpp"
#include "Framework/Objects.hpp"
#include "Framework/Resources.hpp"

namespace TUK::Game
{
	using namespace Framework;

	class EnemyTankController : public Component
	{
	public:
		using Component::Component;

		void OnStart() override;
		void OnUpdate() override;
		void OnLateUpdate() override;

		[[nodiscard]] f32 GetMoveSpeed() const noexcept;
		void SetMoveSpeed(f32 value) noexcept;

		[[nodiscard]] f32 GetTiltAlignSpeed() const noexcept;
		void SetTiltAlignSpeed(f32 value) noexcept;

		[[nodiscard]] f32 GetWanderInterval() const noexcept;
		void SetWanderInterval(f32 value) noexcept;

		[[nodiscard]] f32 GetDetectionRange() const noexcept;
		void SetDetectionRange(f32 value) noexcept;

		[[nodiscard]] f32 GetReleaseRange() const noexcept;
		void SetReleaseRange(f32 value) noexcept;

		[[nodiscard]] f32 GetAttackRange() const noexcept;
		void SetAttackRange(f32 value) noexcept;

		[[nodiscard]] f32 GetFireInterval() const noexcept;
		void SetFireInterval(f32 value) noexcept;

		[[nodiscard]] f32 GetProjectileSpeed() const noexcept;
		void SetProjectileSpeed(f32 value) noexcept;

		[[nodiscard]] f32 GetProjectileLifetime() const noexcept;
		void SetProjectileLifetime(f32 value) noexcept;

		[[nodiscard]] f32 GetBarrageAngle() const noexcept;
		void SetBarrageAngle(f32 value) noexcept;

		[[nodiscard]] const Vector3D<f32>& GetMuzzleOffset() const noexcept;
		void SetMuzzleOffset(const Vector3D<f32>& value) noexcept;

		[[nodiscard]] const Vector3D<f32>& GetAimOffset() const noexcept;
		void SetAimOffset(const Vector3D<f32>& value) noexcept;

		[[nodiscard]] GameObject* GetTarget() const noexcept;
		void SetTarget(GameObject* value) noexcept;

		[[nodiscard]] i32 GetHealth() const noexcept;
		void SetHealth(i32 value) noexcept;
		[[nodiscard]] bool IsAlive() const noexcept;
		bool TakeDamage(i32 amount = 1) noexcept;

	private:
		void ChooseWanderDirection();
		void UpdateTarget();
		void Fire(const Vector3D<f32>& direction);

		static constexpr i32 DEFAULT_HEALTH = 5;

		static constexpr f32 DEFAULT_MOVE_SPEED = 8.0f;
		static constexpr f32 DEFAULT_TILT_ALIGN_SPEED = 10.0f;
		static constexpr f32 DEFAULT_WANDER_INTERVAL = 2.0f;

		static constexpr f32 DEFAULT_DETECTION_RANGE = 20.0f;
		static constexpr f32 DEFAULT_RELEASE_RANGE = 30.0f;
		static constexpr f32 DEFAULT_ATTACK_RANGE = 15.0f;
		static constexpr f32 DEFAULT_FIRE_INTERVAL = 1.0f;
		static constexpr f32 DEFAULT_BARRAGE_ANGLE = 8.0f;

		static constexpr f32 DEFAULT_PROJECTILE_SPEED = 120.0f;
		static constexpr f32 DEFAULT_PROJECTILE_LIFETIME = 5.0f;
		static inline const Vector3D<f32> DEFAULT_MUZZLE_OFFSET{ 0.0f, 2.15f, 5.18f };
		static inline const Vector3D<f32> DEFAULT_AIM_OFFSET{ 0.0f, 1.0f, 0.0f };

		i32 health = DEFAULT_HEALTH;

		f32 moveSpeed = DEFAULT_MOVE_SPEED;
		f32 tiltAlignSpeed = DEFAULT_TILT_ALIGN_SPEED;
		f32 wanderInterval = DEFAULT_WANDER_INTERVAL;

		f32 detectionRange = DEFAULT_DETECTION_RANGE;
		f32 releaseRange = DEFAULT_RELEASE_RANGE;
		f32 attackRange = DEFAULT_ATTACK_RANGE;
		f32 fireInterval = DEFAULT_FIRE_INTERVAL;
		f32 barrageAngle = DEFAULT_BARRAGE_ANGLE;

		f32 projectileSpeed = DEFAULT_PROJECTILE_SPEED;
		f32 projectileLifetime = DEFAULT_PROJECTILE_LIFETIME;
		Vector3D<f32> muzzleOffset = DEFAULT_MUZZLE_OFFSET;
		Vector3D<f32> aimOffset = DEFAULT_AIM_OFFSET;

		f32 wanderTimeRemaining = 0.0f;
		f32 fireTimeRemaining = 0.0f;
		Vector3D<f32> wanderDirection = Vector3D<f32>::GetForward();
		Vector3D<f32> smoothedUp = Vector3D<f32>::GetUp();
		GameObject* target = nullptr;

		const MeshAsset* projectileMesh = nullptr;
		const MaterialAsset* projectileMaterial = nullptr;
	};

	inline f32 EnemyTankController::GetMoveSpeed() const noexcept
	{
		return moveSpeed;
	}

	inline void EnemyTankController::SetMoveSpeed(f32 value) noexcept
	{
		moveSpeed = value;
	}

	inline f32 EnemyTankController::GetTiltAlignSpeed() const noexcept
	{
		return tiltAlignSpeed;
	}

	inline void EnemyTankController::SetTiltAlignSpeed(f32 value) noexcept
	{
		tiltAlignSpeed = value;
	}

	inline f32 EnemyTankController::GetWanderInterval() const noexcept
	{
		return wanderInterval;
	}

	inline void EnemyTankController::SetWanderInterval(f32 value) noexcept
	{
		wanderInterval = value;
	}

	inline f32 EnemyTankController::GetDetectionRange() const noexcept
	{
		return detectionRange;
	}

	inline void EnemyTankController::SetDetectionRange(f32 value) noexcept
	{
		detectionRange = value;
	}

	inline f32 EnemyTankController::GetReleaseRange() const noexcept
	{
		return releaseRange;
	}

	inline void EnemyTankController::SetReleaseRange(f32 value) noexcept
	{
		releaseRange = value;
	}

	inline f32 EnemyTankController::GetAttackRange() const noexcept
	{
		return attackRange;
	}

	inline void EnemyTankController::SetAttackRange(f32 value) noexcept
	{
		attackRange = value;
	}

	inline f32 EnemyTankController::GetFireInterval() const noexcept
	{
		return fireInterval;
	}

	inline void EnemyTankController::SetFireInterval(f32 value) noexcept
	{
		fireInterval = value;
	}

	inline f32 EnemyTankController::GetProjectileSpeed() const noexcept
	{
		return projectileSpeed;
	}

	inline void EnemyTankController::SetProjectileSpeed(f32 value) noexcept
	{
		projectileSpeed = value;
	}

	inline f32 EnemyTankController::GetProjectileLifetime() const noexcept
	{
		return projectileLifetime;
	}

	inline void EnemyTankController::SetProjectileLifetime(f32 value) noexcept
	{
		projectileLifetime = value;
	}

	inline f32 EnemyTankController::GetBarrageAngle() const noexcept
	{
		return barrageAngle;
	}

	inline void EnemyTankController::SetBarrageAngle(f32 value) noexcept
	{
		barrageAngle = value;
	}

	inline const Vector3D<f32>& EnemyTankController::GetMuzzleOffset() const noexcept
	{
		return muzzleOffset;
	}

	inline void EnemyTankController::SetMuzzleOffset(const Vector3D<f32>& value) noexcept
	{
		muzzleOffset = value;
	}

	inline const Vector3D<f32>& EnemyTankController::GetAimOffset() const noexcept
	{
		return aimOffset;
	}

	inline void EnemyTankController::SetAimOffset(const Vector3D<f32>& value) noexcept
	{
		aimOffset = value;
	}

	inline void EnemyTankController::SetTarget(GameObject* value) noexcept
	{
		target = value;
	}

	inline i32 EnemyTankController::GetHealth() const noexcept
	{
		return health;
	}

	inline void EnemyTankController::SetHealth(i32 value) noexcept
	{
		health = std::max(0, value);
	}

	inline bool EnemyTankController::IsAlive() const noexcept
	{
		return health > 0;
	}

	inline bool EnemyTankController::TakeDamage(i32 amount) noexcept
	{
		health = std::max(0, health - amount);
		return health <= 0;
	}
}
