#pragma once

#include <random>

#include "Component.h"
#include "Vector3D.h"

class Transform;

class EnemyController final : public Component
{
public:
	EnemyController() = default;
	~EnemyController() override = default;

	[[nodiscard]] float GetVisualYawOffsetDegrees() const noexcept;
	void SetVisualYawOffsetDegrees(float yawDegrees_) noexcept;
	void Die();

protected:
	void OnAwake() override;
	void OnUpdate() override;
	void OnCollisionEnter(class Collider* other_) override;

private:
	void AcquirePlayerTarget();
	[[nodiscard]] bool HasLineOfSightToPlayer(Transform* transform_) const;
	void UpdateWander(float deltaTime_, Transform* transform_);
	void PickNewWanderState();
	void MoveInDirection(Transform* transform_, const Vector3D& direction_, float speed_, float deltaTime_);
	void FaceDirection(Transform* transform_, const Vector3D& direction_);
	void FireProjectile(Transform* transform_, const Vector3D& direction_);
	void SpawnExplosionParticles(const Vector3D& center_);

	Transform* playerTarget{ nullptr };
	Vector3D wanderDirection{ Vector3D::GetForward() };
	float wanderTimeRemaining{ 0.0f };
	float attackTimer{ 0.0f };

	float moveSpeed{ 8.0f };
	float detectionRange{ 120.0f };
	float chaseStopDistance{ 35.0f };
	float attackRange{ 60.0f };
	float attackCooldown{ 1.0f };
	float projectileSpeed{ 70.0f };
	float projectileLifetime{ 3.0f };
	float wanderDurationMin{ 1.5f };
	float wanderDurationMax{ 4.0f };
	float visualYawOffsetDegrees{ 0.0f };
	float turnSpeed{ 360.0f };
	bool isDying{ false };

	std::mt19937 randomEngine{ std::random_device{}() };
};
