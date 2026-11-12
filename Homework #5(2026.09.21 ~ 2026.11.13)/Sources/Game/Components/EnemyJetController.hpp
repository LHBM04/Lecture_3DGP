#pragma once

#include "Framework/Objects.hpp"
#include "Framework/Resources.hpp"

namespace TUK::Game
{
	using namespace Framework;

	class EnemyJetController final : public Component
	{
	public:
		using Component::Component;

		void OnStart() override;
		void OnUpdate() override;
		void OnLateUpdate() override;

		[[nodiscard]] f32 GetMoveSpeed() const noexcept;
		void SetMoveSpeed(f32 value) noexcept;

		[[nodiscard]] f32 GetRotationSpeed() const noexcept;
		void SetRotationSpeed(f32 value) noexcept;

		[[nodiscard]] f32 GetDirectionChangeInterval() const noexcept;
		void SetDirectionChangeInterval(f32 value) noexcept;

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

		[[nodiscard]] GameObject* GetTarget() const noexcept;
		void SetTarget(GameObject* value) noexcept;

		[[nodiscard]] f32 GetMinAltitude() const noexcept;
		void SetMinAltitude(f32 value) noexcept;

		[[nodiscard]] i32 GetHealth() const noexcept;
		void SetHealth(i32 value) noexcept;
		[[nodiscard]] bool IsAlive() const noexcept;
		bool TakeDamage(i32 amount = 1) noexcept;

	private:
		void SelectRandomDirection();
		void UpdateTarget();
		void Fire(const Vector3D<f32>& direction);

		static constexpr i32 DEFAULT_HEALTH = 5;

		static constexpr f32 DEFAULT_MOVE_SPEED = 14.0f;
		static constexpr f32 DEFAULT_ROTATION_SPEED = 90.0f;
		static constexpr f32 DEFAULT_DIRECTION_CHANGE_INTERVAL = 3.0f;
		static constexpr f32 DEFAULT_MIN_ALTITUDE = 25.0f;

		static constexpr f32 DEFAULT_DETECTION_RANGE = 50.0f;
		static constexpr f32 DEFAULT_RELEASE_RANGE = 70.0f;
		static constexpr f32 DEFAULT_ATTACK_RANGE = 35.0f;
		static constexpr f32 DEFAULT_FIRE_INTERVAL = 1.0f;
		static constexpr f32 DEFAULT_BARRAGE_ANGLE = 8.0f;

		static constexpr f32 DEFAULT_PROJECTILE_SPEED = 160.0f;
		static constexpr f32 DEFAULT_PROJECTILE_LIFETIME = 5.0f;
		static inline const Vector3D<f32> DEFAULT_MUZZLE_OFFSET{ 0.0f, 0.0f, 5.2f };

		i32 health = DEFAULT_HEALTH;

		f32 moveSpeed = DEFAULT_MOVE_SPEED;
		f32 rotationSpeed = DEFAULT_ROTATION_SPEED;
		f32 directionChangeInterval = DEFAULT_DIRECTION_CHANGE_INTERVAL;
		f32 minAltitude = DEFAULT_MIN_ALTITUDE;

		f32 detectionRange = DEFAULT_DETECTION_RANGE;
		f32 releaseRange = DEFAULT_RELEASE_RANGE;
		f32 attackRange = DEFAULT_ATTACK_RANGE;
		f32 fireInterval = DEFAULT_FIRE_INTERVAL;
		f32 barrageAngle = DEFAULT_BARRAGE_ANGLE;

		f32 projectileSpeed = DEFAULT_PROJECTILE_SPEED;
		f32 projectileLifetime = DEFAULT_PROJECTILE_LIFETIME;
		Vector3D<f32> muzzleOffset = DEFAULT_MUZZLE_OFFSET;

		f32 directionChangeTimeRemaining = 0.0f;
		f32 fireTimeRemaining = 0.0f;
		Quaternion<f32> targetRotation{};
		GameObject* target = nullptr;

		const MeshAsset* projectileMesh = nullptr;
		const MaterialAsset* projectileMaterial = nullptr;
	};

	inline f32 EnemyJetController::GetMoveSpeed() const noexcept
	{
		return moveSpeed;
	}

	inline void EnemyJetController::SetMoveSpeed(f32 value) noexcept
	{
		moveSpeed = value;
	}

	inline f32 EnemyJetController::GetRotationSpeed() const noexcept
	{
		return rotationSpeed;
	}

	inline void EnemyJetController::SetRotationSpeed(f32 value) noexcept
	{
		rotationSpeed = value;
	}

	inline f32 EnemyJetController::GetDirectionChangeInterval() const noexcept
	{
		return directionChangeInterval;
	}

	inline void EnemyJetController::SetDirectionChangeInterval(f32 value) noexcept
	{
		directionChangeInterval = value;
	}

	inline f32 EnemyJetController::GetDetectionRange() const noexcept
	{
		return detectionRange;
	}

	inline void EnemyJetController::SetDetectionRange(f32 value) noexcept
	{
		detectionRange = value;
	}

	inline f32 EnemyJetController::GetReleaseRange() const noexcept
	{
		return releaseRange;
	}

	inline void EnemyJetController::SetReleaseRange(f32 value) noexcept
	{
		releaseRange = value;
	}

	inline f32 EnemyJetController::GetAttackRange() const noexcept
	{
		return attackRange;
	}

	inline void EnemyJetController::SetAttackRange(f32 value) noexcept
	{
		attackRange = value;
	}

	inline f32 EnemyJetController::GetFireInterval() const noexcept
	{
		return fireInterval;
	}

	inline void EnemyJetController::SetFireInterval(f32 value) noexcept
	{
		fireInterval = value;
	}

	inline f32 EnemyJetController::GetProjectileSpeed() const noexcept
	{
		return projectileSpeed;
	}

	inline void EnemyJetController::SetProjectileSpeed(f32 value) noexcept
	{
		projectileSpeed = value;
	}

	inline f32 EnemyJetController::GetProjectileLifetime() const noexcept
	{
		return projectileLifetime;
	}

	inline void EnemyJetController::SetProjectileLifetime(f32 value) noexcept
	{
		projectileLifetime = value;
	}

	inline f32 EnemyJetController::GetBarrageAngle() const noexcept
	{
		return barrageAngle;
	}

	inline void EnemyJetController::SetBarrageAngle(f32 value) noexcept
	{
		barrageAngle = value;
	}

	inline const Vector3D<f32>& EnemyJetController::GetMuzzleOffset() const noexcept
	{
		return muzzleOffset;
	}

	inline void EnemyJetController::SetMuzzleOffset(const Vector3D<f32>& value) noexcept
	{
		muzzleOffset = value;
	}

	inline void EnemyJetController::SetTarget(GameObject* value) noexcept
	{
		target = value;
	}

	inline f32 EnemyJetController::GetMinAltitude() const noexcept
	{
		return minAltitude;
	}

	inline void EnemyJetController::SetMinAltitude(f32 value) noexcept
	{
		minAltitude = std::max(0.0f, value);
	}

	inline i32 EnemyJetController::GetHealth() const noexcept
	{
		return health;
	}

	inline void EnemyJetController::SetHealth(i32 value) noexcept
	{
		health = std::max(0, value);
	}

	inline bool EnemyJetController::IsAlive() const noexcept
	{
		return health > 0;
	}

	inline bool EnemyJetController::TakeDamage(i32 amount) noexcept
	{
		health = std::max(0, health - amount);
		return health <= 0;
	}
}
