#include "Precompiled.hpp"
#include "ProjectileController.hpp"

#include "EnemyJetController.hpp"
#include "EnemyTankController.hpp"
#include "PlayerJetController.hpp"
#include "PlayerTankController.hpp"

#include "Framework/Graphics.hpp"
#include "Framework/Resources.hpp"

#include "../Core/Assets.hpp"
#include "../Core/Time.hpp"

namespace TUK::Game
{
	namespace
	{
		constexpr u32 EXPLOSION_COLUMNS = 8;
		constexpr u32 EXPLOSION_ROWS = 8;
		constexpr f32 EXPLOSION_DURATION = 0.8f;

		void SpawnExplosionBillboard(Scene* scene, const Vector3D<f32>& position, f32 size) noexcept
		{
			if (!scene)
			{
				return;
			}

			GameObject* explosion = scene->Instantiate();
			explosion->SetName("ExplosionBillboard");

			Transform* transform = explosion->AddComponent<Transform>();
			transform->SetLocalPosition(position);

			BillboardRenderer* billboard = explosion->AddComponent<BillboardRenderer>();
			billboard->SetMaterial(Assets::LoadAsset<MaterialAsset>("Resources/Materials/Billboard.tukmat"));
			billboard->SetTexture(Assets::LoadAsset<TextureAsset>("Resources/Textures/Fire_Explosion.dds"));
			billboard->SetSize(Vector2D<f32>(size, size));
			billboard->Play(EXPLOSION_DURATION, EXPLOSION_COLUMNS, EXPLOSION_ROWS, false, true);
		}
	}

	void ProjectileController::OnStart()
	{
		elapsedTime = 0.0f;
	}

	void ProjectileController::OnUpdate()
	{
		GameObject* owner = GetOwner();
		const f32 previousTime = elapsedTime;
		elapsedTime = std::min(lifetime, elapsedTime + Time::GetDeltaTime());

		const Vector3D<f32> previousPosition = launchPosition + launchDirection * (moveSpeed * previousTime);
		const Vector3D<f32> position = launchPosition + launchDirection * (moveSpeed * elapsedTime);
		f32 nearestDistance = moveSpeed * (elapsedTime - previousTime);
		GameObject* hitObject = nullptr;

		if (Scene* scene = owner->GetScene(); scene && !targetTag.empty() && nearestDistance > 0.0f)
		{
			for (GameObject* candidate : scene->GetObjects())
			{
				if (candidate->IsDestroyed() || !candidate->IsEnabled())
				{
					continue;
				}
				if (!candidate->CompareTag(targetTag) && !candidate->CompareTag("Terrain"))
				{
					continue;
				}

				for (Component* component : candidate->GetComponents())
				{
					const Collider* collider = dynamic_cast<const Collider*>(component);
					f32 hitDistance = 0.0f;
					if (collider && collider->Raycast(previousPosition, launchDirection, hitDistance, nearestDistance))
					{
						nearestDistance = hitDistance;
						hitObject = candidate;
					}
				}
			}
		}

		if (hitObject)
		{
			const Vector3D<f32> hitPosition = previousPosition + launchDirection * nearestDistance;
			const bool isTargetHit = hitObject->CompareTag(targetTag);
			SpawnExplosionBillboard(owner->GetScene(), hitPosition, isTargetHit ? 8.0f : 5.0f);

			if (isTargetHit)
			{
				bool shouldDestroy = true;
				if (EnemyTankController* enemyTank = hitObject->GetComponent<EnemyTankController>())
				{
					shouldDestroy = enemyTank->TakeDamage(1);
					LOG_INFO("적 전차 피격! 현재 체력={}", enemyTank->GetHealth());
				}
				else if (EnemyJetController* enemyJet = hitObject->GetComponent<EnemyJetController>())
				{
					shouldDestroy = enemyJet->TakeDamage(1);
					LOG_INFO("적 전투기 피격! 현재 체력={}", enemyJet->GetHealth());
				}
				else if (PlayerTankController* playerTank = hitObject->GetComponent<PlayerTankController>())
				{
					shouldDestroy = playerTank->TakeDamage(1);
					LOG_INFO("플레이어 전차 피격! 현재 체력={}", playerTank->GetHealth());
				}
				else if (PlayerJetController* playerJet = hitObject->GetComponent<PlayerJetController>())
				{
					shouldDestroy = playerJet->TakeDamage(1);
					LOG_INFO("플레이어 전투기 피격! 현재 체력={}", playerJet->GetHealth());
				}

				LOG_INFO("발사체 명중: 발사체 이름={}, 태그={}; 대상 이름={}, 태그={}", owner->GetName(), owner->GetTag(), hitObject->GetName(), hitObject->GetTag());
				if (shouldDestroy)
				{
					DestroyHierarchy(hitObject);
				}
			}

			owner->Destroy();
			return;
		}

		Transform* transform = owner->GetComponent<Transform>();
		ASSERT_FMT(transform, "발사체의 Transform 컴포넌트를 찾을 수 없습니다.");

		transform->SetWorldPosition(position);

		if (elapsedTime >= lifetime)
		{
			owner->Destroy();
		}
	}

	void ProjectileController::DestroyHierarchy(GameObject* gameObject) noexcept
	{
		if (Transform* transform = gameObject->GetComponent<Transform>())
		{
			for (Transform* child : transform->GetChildren())
			{
				DestroyHierarchy(child->GetOwner());
			}
		}

		gameObject->Destroy();
	}
}
