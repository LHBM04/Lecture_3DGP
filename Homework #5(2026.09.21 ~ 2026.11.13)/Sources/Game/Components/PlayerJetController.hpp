#pragma once

#include "Framework/Objects.hpp"
#include "Framework/Platform.hpp"
#include "Framework/Resources.hpp"

namespace TUK::Game
{
	using namespace Framework;

	class PlayerJetController final : public Component
	{
	public:
		using Component::Component;

		void OnStart() override;
		void OnUpdate() override;
		void OnLateUpdate() override;
		void OnDestroy() override;

		[[nodiscard]] f32 GetMoveSpeed() const noexcept;
		void SetMoveSpeed(f32 value) noexcept;

		[[nodiscard]] f32 GetBoostMultiplier() const noexcept;
		void SetBoostMultiplier(f32 value) noexcept;

		[[nodiscard]] f32 GetRotationSpeed() const noexcept;
		void SetRotationSpeed(f32 value) noexcept;

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
		void OnMovePerformed(const InputActionContext& context);
		void Fire(const Vector3D<f32>& direction);

		static constexpr i32 DEFAULT_HEALTH = 10;

		static constexpr f32 DEFAULT_MOVE_SPEED = 20.0f;
		static constexpr f32 DEFAULT_BOOST_MULTIPLIER = 2.0f;
		static constexpr f32 DEFAULT_ROTATION_SPEED = 0.1f;

		static constexpr f32 DEFAULT_PROJECTILE_SPEED = 160.0f;
		static constexpr f32 DEFAULT_PROJECTILE_LIFETIME = 5.0f;
		static inline const Vector3D<f32> DEFAULT_MUZZLE_OFFSET{ 0.0f, 0.0f, 5.5f };

		i32 health = DEFAULT_HEALTH;

		f32 moveSpeed = DEFAULT_MOVE_SPEED;
		f32 boostMultiplier = DEFAULT_BOOST_MULTIPLIER;
		f32 rotationSpeed = DEFAULT_ROTATION_SPEED;

		f32 projectileSpeed = DEFAULT_PROJECTILE_SPEED;
		f32 projectileLifetime = DEFAULT_PROJECTILE_LIFETIME;
		Vector3D<f32> muzzleOffset = DEFAULT_MUZZLE_OFFSET;

		InputAction::ActionCallback::ListenerId moveListenerId = -1;

		const MeshAsset* projectileMesh = nullptr;
		const MaterialAsset* projectileMaterial = nullptr;
	};

	inline f32 PlayerJetController::GetMoveSpeed() const noexcept
	{
		return moveSpeed;
	}

	inline void PlayerJetController::SetMoveSpeed(f32 value) noexcept
	{
		moveSpeed = value;
	}

	inline f32 PlayerJetController::GetBoostMultiplier() const noexcept
	{
		return boostMultiplier;
	}

	inline void PlayerJetController::SetBoostMultiplier(f32 value) noexcept
	{
		boostMultiplier = value;
	}

	inline f32 PlayerJetController::GetRotationSpeed() const noexcept
	{
		return rotationSpeed;
	}

	inline void PlayerJetController::SetRotationSpeed(f32 value) noexcept
	{
		rotationSpeed = value;
	}

	inline f32 PlayerJetController::GetProjectileSpeed() const noexcept
	{
		return projectileSpeed;
	}

	inline void PlayerJetController::SetProjectileSpeed(f32 value) noexcept
	{
		projectileSpeed = value;
	}

	inline f32 PlayerJetController::GetProjectileLifetime() const noexcept
	{
		return projectileLifetime;
	}

	inline void PlayerJetController::SetProjectileLifetime(f32 value) noexcept
	{
		projectileLifetime = value;
	}

	inline const Vector3D<f32>& PlayerJetController::GetMuzzleOffset() const noexcept
	{
		return muzzleOffset;
	}

	inline void PlayerJetController::SetMuzzleOffset(const Vector3D<f32>& value) noexcept
	{
		muzzleOffset = value;
	}

	inline i32 PlayerJetController::GetHealth() const noexcept
	{
		return health;
	}

	inline void PlayerJetController::SetHealth(i32 value) noexcept
	{
		health = std::max(0, value);
	}

	inline bool PlayerJetController::IsAlive() const noexcept
	{
		return health > 0;
	}

	inline bool PlayerJetController::TakeDamage(i32 amount) noexcept
	{
		health = std::max(0, health - amount);
		return health <= 0;
	}
}
