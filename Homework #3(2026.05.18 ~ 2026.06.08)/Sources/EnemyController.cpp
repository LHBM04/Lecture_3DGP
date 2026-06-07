#include "Precompiled.h"
#include "EnemyController.h"

#include <cmath>
#include <numbers>

#include "Collider.h"
#include "EnemyProjectile.h"
#include "ExplodeParticle.h"
#include "GameObject.h"
#include "MathF.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "PhysicsSystem.h"
#include "Quaternion.h"
#include "ResourceSystem.h"
#include "Scene.h"
#include "SphereCollider.h"
#include "TimeSystem.h"
#include "Transform.h"

float EnemyController::GetVisualYawOffsetDegrees() const noexcept
{
	return visualYawOffsetDegrees;
}

void EnemyController::SetVisualYawOffsetDegrees(float yawDegrees_) noexcept
{
	visualYawOffsetDegrees = yawDegrees_;
}

void EnemyController::Die()
{
	GameObject* const owner{ GetOwner() };
	if (owner == nullptr || owner->IsDestroyed() || isDying)
	{
		return;
	}

	isDying = true;

	if (Transform* const transform{ owner->GetComponent<Transform>() }; transform != nullptr)
	{
		SpawnExplosionParticles(transform->GetWorldPosition());
	}

	if (Scene* const scene{ owner->GetScene() })
	{
		scene->Destroy(owner);
	}
}

void EnemyController::OnAwake()
{
	PickNewWanderState();
	AcquirePlayerTarget();
}

void EnemyController::OnUpdate()
{
	GameObject* const owner{ GetOwner() };
	if (owner == nullptr)
	{
		return;
	}

	Transform* const transform{ owner->GetComponent<Transform>() };
	if (transform == nullptr)
	{
		return;
	}

	attackTimer += TimeSystem::GetInstance().GetDeltaTime();
	AcquirePlayerTarget();

	const float deltaTime{ TimeSystem::GetInstance().GetDeltaTime() };
	if (playerTarget == nullptr)
	{
		UpdateWander(deltaTime, transform);
		return;
	}

	const Vector3D toPlayerFull{ playerTarget->GetWorldPosition() - transform->GetWorldPosition() };
	const Vector3D toPlayer{ Vector3D::ProjectOnPlane(toPlayerFull, Vector3D::GetUp()) };
	const float distanceToPlayer{ toPlayer.GetMagnitude() };

	if (distanceToPlayer > detectionRange || !HasLineOfSightToPlayer(transform))
	{
		UpdateWander(deltaTime, transform);
		return;
	}

	if (!toPlayer.IsZero())
	{
		FaceDirection(transform, toPlayer);
	}

	if (distanceToPlayer > chaseStopDistance)
	{
		MoveInDirection(transform, toPlayer, moveSpeed, deltaTime);
	}

	if (distanceToPlayer <= attackRange && attackTimer >= attackCooldown)
	{
		FireProjectile(transform, toPlayer.IsZero() ? transform->GetWorldRotation() * Vector3D::GetForward() : toPlayer);
		attackTimer = 0.0f;
	}
}

void EnemyController::OnCollisionEnter(Collider* other_)
{
	GameObject* const owner{ GetOwner() };
	if (owner == nullptr || owner->IsDestroyed() || other_ == nullptr || other_->GetOwner() == nullptr)
	{
		return;
	}

	GameObject* const otherOwner{ other_->GetOwner() };
	if (otherOwner->GetTag() != L"PlayerProjectile")
	{
		return;
	}

	Die();
}

void EnemyController::AcquirePlayerTarget()
{
	if (playerTarget != nullptr && playerTarget->GetOwner() != nullptr && !playerTarget->GetOwner()->IsDestroyed())
	{
		return;
	}

	GameObject* const owner{ GetOwner() };
	if (owner == nullptr)
	{
		playerTarget = nullptr;
		return;
	}

	Scene* const scene{ owner->GetScene() };
	if (scene == nullptr)
	{
		playerTarget = nullptr;
		return;
	}

	GameObject* const playerObject{ scene->FindObjectWithTag(L"Player") };
	playerTarget = playerObject != nullptr ? playerObject->GetComponent<Transform>() : nullptr;
}

bool EnemyController::HasLineOfSightToPlayer(Transform* transform_) const
{
	if (transform_ == nullptr || playerTarget == nullptr)
	{
		return false;
	}

	const Vector3D eyePosition{ transform_->GetWorldPosition() + Vector3D(0.0f, 2.0f, 0.0f) };
	const Vector3D targetPosition{ playerTarget->GetWorldPosition() + Vector3D(0.0f, 2.0f, 0.0f) };
	const Vector3D toPlayer{ targetPosition - eyePosition };
	if (toPlayer.IsZero())
	{
		return true;
	}

	const Vector3D rayDirection{ toPlayer.GetNormalized() };
	const Vector3D rayOrigin{ eyePosition + rayDirection * 6.0f };

	PhysicsSystem::RaycastHit hitInfo{};
	if (!PhysicsSystem::GetInstance().Raycast(rayOrigin, rayDirection, hitInfo, detectionRange))
	{
		return false;
	}

	return hitInfo.gameObject != nullptr && hitInfo.gameObject->GetTag() == L"Player";
}

void EnemyController::UpdateWander(float deltaTime_, Transform* transform_)
{
	wanderTimeRemaining -= deltaTime_;
	if (wanderTimeRemaining <= 0.0f || wanderDirection.IsZero())
	{
		PickNewWanderState();
	}

	MoveInDirection(transform_, wanderDirection, moveSpeed * 0.6f, deltaTime_);
	FaceDirection(transform_, wanderDirection);
}

void EnemyController::PickNewWanderState()
{
	std::uniform_real_distribution<float> angleDistribution(0.0f, std::numbers::pi_v<float> * 2.0f);
	std::uniform_real_distribution<float> durationDistribution(wanderDurationMin, wanderDurationMax);

	const float angle{ angleDistribution(randomEngine) };
	wanderDirection = Vector3D(std::sin(angle), 0.0f, std::cos(angle)).GetNormalized();
	wanderTimeRemaining = durationDistribution(randomEngine);
}

void EnemyController::MoveInDirection(Transform* transform_, const Vector3D& direction_, float speed_, float deltaTime_)
{
	if (transform_ == nullptr || direction_.IsZero())
	{
		return;
	}

	const Vector3D moveDirection{ direction_.GetNormalized() };
	transform_->SetWorldPosition(transform_->GetWorldPosition() + moveDirection * speed_ * deltaTime_);
}

void EnemyController::FaceDirection(Transform* transform_, const Vector3D& direction_)
{
	if (transform_ == nullptr || direction_.IsZero())
	{
		return;
	}

	// 수평 방향만 고려하여 몸체가 위아래로 기울어지는 현상을 방지합니다.
	Vector3D horizontalDir{ direction_.x, 0.0f, direction_.z };
	if (horizontalDir.IsZero())
	{
		return;
	}

	const Quaternion targetRotation{ Quaternion::LookRotation(horizontalDir.GetNormalized(), Vector3D::GetUp()) };
	const Quaternion visualOffset{ Quaternion::Euler(0.0f, visualYawOffsetDegrees, 0.0f) };
	
	// Apply visual offset (local) then look rotation (world)
	const Quaternion finalTarget{ visualOffset * targetRotation };
	
	const float deltaTime{ TimeSystem::GetInstance().GetDeltaTime() };
	const Quaternion currentRotation{ transform_->GetWorldRotation() };
	
	transform_->SetWorldRotation(Quaternion::RotateTowards(currentRotation, finalTarget, turnSpeed * deltaTime));
}

void EnemyController::FireProjectile(Transform* transform_, const Vector3D& direction_)
{
	GameObject* const owner{ GetOwner() };
	if (owner == nullptr || transform_ == nullptr)
	{
		return;
	}

	Scene* const scene{ owner->GetScene() };
	if (scene == nullptr)
	{
		return;
	}

	const Vector3D projectileDirection{ direction_.GetNormalized() };
	if (projectileDirection.IsZero())
	{
		return;
	}

	GameObject* const projectile{ scene->Instantiate(
		transform_->GetWorldPosition() + projectileDirection * 8.0f + Vector3D(0.0f, 2.0f, 0.0f),
		Quaternion::LookRotation(projectileDirection, Vector3D::GetUp())) };
	projectile->SetName(L"EnemyProjectile");
	projectile->SetTag(L"EnemyProjectile");

	Mesh* const mesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/PlayerProjectile.bin") };
	Material* const material{ ResourceSystem::GetInstance().GetResource<Material>(L"Resources/Materials/EnemyProjectile.bin") };
	if (mesh == nullptr || material == nullptr)
	{
		scene->Destroy(projectile);
		return;
	}

	MeshRenderer* const meshRenderer{ projectile->AddComponent<MeshRenderer>() };
	meshRenderer->SetMesh(mesh);
	meshRenderer->SetMaterial(material);

	if (Transform* const projectileTransform{ projectile->GetComponent<Transform>() }; projectileTransform != nullptr)
	{
		projectileTransform->SetLocalScale(Vector3D(4.0f, 4.0f, 4.0f));
	}

	EnemyProjectile* const projectileLogic{ projectile->AddComponent<EnemyProjectile>() };
	projectileLogic->SetDirection(projectileDirection);
	projectileLogic->SetSpeed(projectileSpeed);
	projectileLogic->SetLifeTime(projectileLifetime);

	SphereCollider* const projectileCollider{ projectile->AddComponent<SphereCollider>() };
	projectileCollider->SetRadius(0.2f);
	projectileCollider->SetStatic(false);
	projectileCollider->UpdateVolume();
}

void EnemyController::SpawnExplosionParticles(const Vector3D& center_)
{
	GameObject* const owner{ GetOwner() };
	if (owner == nullptr)
	{
		return;
	}

	Scene* const scene{ owner->GetScene() };
	if (scene == nullptr)
	{
		return;
	}

	Mesh* const particleMesh{ ResourceSystem::GetInstance().GetResource<Mesh>(L"Resources/Meshes/Particle.bin") };
	Material* const particleMaterial{ ResourceSystem::GetInstance().GetResource<Material>(L"Resources/Materials/ExplodeParticle.bin") };
	if (particleMesh == nullptr || particleMaterial == nullptr)
	{
		return;
	}

	constexpr int particleCount{ 24 };
	for (int index = 0; index < particleCount; ++index)
	{
		std::uniform_real_distribution<float> angleDist(0.0f, Mathf::Pi * 2.0f);
		std::uniform_real_distribution<float> speedDist(15.0f, 25.0f);
		std::uniform_real_distribution<float> verticalDist(8.0f, 18.0f);

		const float angle{ angleDist(randomEngine) };
		const float speed{ speedDist(randomEngine) };
		const Vector3D horizontalDirection{ std::cos(angle), 0.0f, std::sin(angle) };
		const Vector3D velocity{
			horizontalDirection.x * speed,
			verticalDist(randomEngine),
			horizontalDirection.z * speed };

		GameObject* const particle{ scene->Instantiate(center_, Quaternion::GetIdentity()) };
		particle->SetName(L"ExplodeParticle");
		particle->SetTag(L"Particle");

		if (Transform* const particleTransform{ particle->GetComponent<Transform>() }; particleTransform != nullptr)
		{
			particleTransform->SetLocalScale(Vector3D(2.0f, 2.0f, 2.0f));
		}

		MeshRenderer* const meshRenderer{ particle->AddComponent<MeshRenderer>() };
		meshRenderer->SetMesh(particleMesh);
		meshRenderer->SetMaterial(particleMaterial);

		ExplodeParticle* const explodeParticle{ particle->AddComponent<ExplodeParticle>() };
		explodeParticle->SetVelocity(velocity);
		explodeParticle->SetLifeTime(0.75f);
		explodeParticle->SetGravity(-22.0f);
	}
}
