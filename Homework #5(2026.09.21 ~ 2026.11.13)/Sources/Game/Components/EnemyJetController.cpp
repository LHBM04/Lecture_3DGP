#include "Precompiled.hpp"
#include "EnemyJetController.hpp"

#include "ProjectileController.hpp"

#include "Framework/Graphics.hpp"
#include "Framework/Resources.hpp"

#include "../Core/Assets.hpp"
#include "../Core/Time.hpp"

namespace TUK::Game
{
	void EnemyJetController::OnStart()
	{
		SelectRandomDirection();
		projectileMesh = Assets::LoadAsset<MeshAsset>("Resources/Meshes/SM_Bullet_01/SM_Bullet_01.tukmesh");
		projectileMaterial = Assets::LoadAsset<MaterialAsset>("Resources/Materials/Bullets_M.tukmat");
	}

	void EnemyJetController::OnUpdate()
	{
		GameObject* owner = GetOwner();
		ASSERT_FMT(owner, "EnemyJetController 컴포넌트가 부착된 게임 오브젝트를 찾을 수 없습니다.");

		Transform* transform = owner->GetComponent<Transform>();
		if (!transform)
		{
			LOG_ERROR("유효한 Transform 컴포넌트를 찾을 수 없습니다.");
			return;
		}

		UpdateTarget();

		const f32 deltaTime = Time::GetDeltaTime();
		const Vector3D<f32> position = transform->GetWorldPosition();
		bool isChasing = false;

		const GameObject* player = GetTarget();
		const Transform* playerTransform = player ? player->GetComponent<Transform>() : nullptr;
		if (playerTransform)
		{
			const Vector3D<f32> direction = playerTransform->GetWorldPosition() - position;
			isChasing = true;
			if (!direction.IsZero())
			{
				const Vector3D<f32> forward = direction.GetNormalized();
				const Vector3D<f32> up = std::abs(forward.Dot(Vector3D<f32>::GetUp())) > 0.99f
					? Vector3D<f32>::GetForward() : Vector3D<f32>::GetUp();
				targetRotation = Quaternion<f32>::LookRotation(forward, up);
			}
		}

		if (isChasing)
		{
			directionChangeTimeRemaining = 0.0f;
		}
		else
		{
			directionChangeTimeRemaining -= deltaTime;
			if (directionChangeTimeRemaining <= 0.0f)
			{
				SelectRandomDirection();
			}
		}

		const Quaternion<f32> rotation = Quaternion<f32>::RotateTowards(transform->GetWorldRotation(), targetRotation, rotationSpeed * deltaTime);
		const Vector3D<f32> forward = rotation * Vector3D<f32>::GetForward();
		Vector3D<f32> nextPosition = position + forward * (moveSpeed * deltaTime);

		if (Scene* scene = owner->GetScene())
		{
			if (const GameObject* terrainObject = scene->FindGameObjectByTag("Terrain"))
			{
				if (const Terrain* terrain = terrainObject->GetComponent<Terrain>(); terrain && terrain->IsValid())
				{
					const f32 terrainHeight = terrain->SampleHeightAtWorldPosition(nextPosition);
					const f32 minAllowedY = terrainHeight + minAltitude;
					if (nextPosition.GetY() < minAllowedY)
					{
						nextPosition.SetY(minAllowedY);

						Vector3D<f32> climbDirection = forward;
						climbDirection.SetY(std::max(0.3f, std::abs(climbDirection.GetY()) + 0.2f));
						climbDirection.Normalize();
						targetRotation = Quaternion<f32>::LookRotation(climbDirection, Vector3D<f32>::GetUp());
					}
				}
			}
		}

		transform->SetWorldRotation(rotation);
		transform->SetWorldPosition(nextPosition);
	}

	void EnemyJetController::OnLateUpdate()
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
			const Vector3D<f32> aimPosition = targetTransform->GetWorldPosition();
			const Vector3D<f32> direction = aimPosition - muzzlePosition;
			if (!direction.IsZero())
			{
				Fire(direction.GetNormalized());
				fireTimeRemaining = fireInterval;
			}
		}
	}

	GameObject* EnemyJetController::GetTarget() const noexcept
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

	void EnemyJetController::SelectRandomDirection()
	{
		const f32 yaw = Random::RandFloat(-std::numbers::pi_v<f32>, std::numbers::pi_v<f32>);
		f32 minHeightRatio = -0.5f;

		if (const GameObject* owner = GetOwner())
		{
			if (const Scene* scene = owner->GetScene())
			{
				if (const GameObject* terrainObject = scene->FindGameObjectByTag("Terrain"))
				{
					if (const Terrain* terrain = terrainObject->GetComponent<Terrain>(); terrain && terrain->IsValid())
					{
						if (const Transform* transform = owner->GetComponent<Transform>())
						{
							const f32 currentY = transform->GetWorldPosition().GetY();
							const f32 terrainY = terrain->SampleHeightAtWorldPosition(transform->GetWorldPosition());
							if (currentY <= terrainY + minAltitude * 1.5f)
							{
								minHeightRatio = 0.1f;
							}
						}
					}
				}
			}
		}

		const f32 height = Random::RandFloat(minHeightRatio, 1.0f);
		const f32 horizontal = std::sqrt(std::max(0.0f, 1.0f - height * height));
		const Vector3D<f32> direction(horizontal * std::sin(yaw), height, horizontal * std::cos(yaw));

		targetRotation = Quaternion<f32>::LookRotation(direction, Vector3D<f32>::GetUp());
		directionChangeTimeRemaining = directionChangeInterval;
	}

	void EnemyJetController::UpdateTarget()
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

	void EnemyJetController::Fire(const Vector3D<f32>& direction)
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
