#include "Precompiled.hpp"
#include "EnemyTankController.hpp"

#include "ProjectileController.hpp"
#include "TerrainAlignment.hpp"

#include "Framework/Graphics.hpp"
#include "Framework/Resources.hpp"

#include "../Core/Assets.hpp"
#include "../Core/Time.hpp"

namespace TUK::Game
{
	void EnemyTankController::OnStart()
	{
		projectileMesh = Assets::LoadAsset<MeshAsset>("Resources/Meshes/SM_Bullet_01/SM_Bullet_01.tukmesh");
		projectileMaterial = Assets::LoadAsset<MaterialAsset>("Resources/Materials/Bullets_M.tukmat");

		if (GameObject* owner = GetOwner())
		{
			if (Transform* transform = owner->GetComponent<Transform>())
			{
				Vector3D<f32> position = transform->GetWorldPosition();
				const Quaternion<f32> alignedRotation = AlignTankToTerrain(
					owner->GetScene(),
					position,
					smoothedUp,
					wanderDirection,
					1.0f);
				transform->SetWorldPosition(position);
				transform->SetWorldRotation(alignedRotation);
			}
		}
	}

	void EnemyTankController::OnUpdate()
	{
		GameObject* owner = GetOwner();
		ASSERT_FMT(owner, "EnemyTankController 컴포넌트가 부착된 게임 오브젝트를 찾을 수 없습니다.");

		Transform* transform = owner->GetComponent<Transform>();
		if (!transform)
		{
			LOG_ERROR("유효한 Transform 컴포넌트를 찾을 수 없습니다.");
			return;
		}

		UpdateTarget();

		const f32 deltaTime = Time::GetDeltaTime();
		const Vector3D<f32> position = transform->GetWorldPosition();
		Vector3D<f32> direction = wanderDirection;
		f32 movementDistance = moveSpeed * deltaTime;
		bool isChasing = false;

		const GameObject* player = GetTarget();
		const Transform* playerTransform = player ? player->GetComponent<Transform>() : nullptr;
		if (playerTransform)
		{
			Vector3D<f32> offset = playerTransform->GetWorldPosition() - position;
			offset.SetY(0.0f);
			const f32 distance = offset.GetMagnitude();
			isChasing = true;
			movementDistance = std::min(movementDistance, distance);
			if (!offset.IsZero())
			{
				direction = offset / distance;
			}
		}

		if (!isChasing)
		{
			wanderTimeRemaining -= deltaTime;
			if (wanderTimeRemaining <= 0.0f)
			{
				ChooseWanderDirection();
			}

			direction = wanderDirection;
		}

		Vector3D<f32> nextPosition = position + direction * movementDistance;
		const f32 blendFactor = ComputeTiltBlendFactor(tiltAlignSpeed, deltaTime);
		const Quaternion<f32> alignedRotation = AlignTankToTerrain(
			owner->GetScene(),
			nextPosition,
			smoothedUp,
			direction,
			blendFactor);

		transform->SetWorldRotation(alignedRotation);
		transform->SetWorldPosition(nextPosition);
	}

	void EnemyTankController::OnLateUpdate()
	{
		const Transform* transform = GetOwner()->GetComponent<Transform>();
		if (!transform)
		{
			return;
		}

		fireTimeRemaining = std::max(0.0f, fireTimeRemaining - Time::GetDeltaTime());

		const GameObject* attackTarget = GetTarget();
		const Transform* targetTransform = attackTarget ? attackTarget->GetComponent<Transform>() : nullptr;
		if (!targetTransform)
		{
			return;
		}

		const Vector3D<f32> offset = targetTransform->GetWorldPosition() - transform->GetWorldPosition();
		if (offset.GetSqrMagnitude() <= attackRange * attackRange && fireTimeRemaining <= 0.0f)
		{
			const Vector3D<f32> muzzlePosition = transform->GetWorldPosition() + transform->GetWorldRotation() * muzzleOffset;
			const Vector3D<f32> aimPosition = targetTransform->GetWorldPosition() + aimOffset;
			const Vector3D<f32> direction = aimPosition - muzzlePosition;
			if (!direction.IsZero())
			{
				Fire(direction.GetNormalized());
				fireTimeRemaining = fireInterval;
			}
		}
	}

	GameObject* EnemyTankController::GetTarget() const noexcept
	{
		const Scene* scene = GetOwner()->GetScene();
		if (target && scene)
		{
			for (const GameObject* gameObject : scene->GetObjects())
			{
				if (gameObject == target)
				{
					return !gameObject->IsDestroyed() && gameObject->IsEnabled() && gameObject->CompareTag("Player") ? target : nullptr;
				}
			}
		}

		return nullptr;
	}

	void EnemyTankController::ChooseWanderDirection()
	{
		const f32 angle = Random::RandFloat(0.0f, 360.0f);
		wanderDirection = Quaternion<f32>::Euler(0.0f, angle, 0.0f) * Vector3D<f32>::GetForward();
		wanderTimeRemaining = wanderInterval;
	}

	void EnemyTankController::UpdateTarget()
	{
		const Transform* transform = GetOwner()->GetComponent<Transform>();
		Scene* scene = GetOwner()->GetScene();
		if (!transform || !scene)
		{
			target = nullptr;
			return;
		}

		target = GetTarget();
		if (target)
		{
			const Transform* targetTransform = target->GetComponent<Transform>();
			if (!targetTransform || (targetTransform->GetWorldPosition() - transform->GetWorldPosition()).GetSqrMagnitude() > releaseRange * releaseRange)
			{
				target = nullptr;
			}
		}

		if (!target)
		{
			GameObject* player = scene->FindGameObjectByTag("Player");
			const Transform* playerTransform = player ? player->GetComponent<Transform>() : nullptr;
			if (playerTransform && player->IsEnabled() && (playerTransform->GetWorldPosition() - transform->GetWorldPosition()).GetSqrMagnitude() <= detectionRange * detectionRange)
			{
				target = player;
			}
		}
	}

	void EnemyTankController::Fire(const Vector3D<f32>& direction)
	{
		Scene* scene = GetOwner()->GetScene();
		const Transform* transform = GetOwner()->GetComponent<Transform>();
		if (!scene || !transform || !projectileMesh || !projectileMaterial)
		{
			return;
		}

		const Vector3D<f32> position = transform->GetWorldPosition() + transform->GetWorldRotation() * muzzleOffset;
		const Vector3D<f32> up = Vector3D<f32>::GetUp();
		const Vector3D<f32> shotUp = std::abs(direction.Dot(up)) > 0.99f ? Vector3D<f32>::GetForward() : up;
		const Quaternion<f32> aimRotation = Quaternion<f32>::LookRotation(direction, shotUp);

		for (i32 index = -1; index <= 1; ++index)
		{
			const Quaternion<f32> rotation = aimRotation * Quaternion<f32>::Euler(0.0f, static_cast<f32>(index) * barrageAngle, 0.0f);
			const Vector3D<f32> shotDirection = rotation * Vector3D<f32>::GetForward();
			const Quaternion<f32> projectileRotation = Quaternion<f32>::Euler(90.0f, 0.0f, 0.0f) * rotation;

			GameObject* projectile = scene->Instantiate(position, projectileRotation);
			projectile->SetName("Enemy Projectile");
			projectile->SetTag("EnemyProjectile");
			projectile->GetComponent<Transform>()->SetLocalScale(Vector3D<f32>(5.0f, 5.0f, 5.0f));

			MeshRenderer* renderer = projectile->AddComponent<MeshRenderer>();
			renderer->SetMesh(projectileMesh);
			renderer->SetMaterial(projectileMaterial);
			renderer->SetColor(ColorRGBA<f32>(1.0f, 0.3f, 0.2f, 1.0f));

			ProjectileController* controller = projectile->AddComponent<ProjectileController>();
			controller->SetLaunchPosition(position);
			controller->SetLaunchDirection(shotDirection);
			controller->SetMoveSpeed(projectileSpeed);
			controller->SetLifetime(projectileLifetime);
			controller->SetTargetTag("Player");
		}
	}
}
