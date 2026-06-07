#include "Precompiled.h"
#include "PlayerProjectile.h"

#include "Collider.h"
#include "EnemyController.h"
#include "GameObject.h"
#include "Scene.h"
#include "TimeSystem.h"
#include "Transform.h"

void PlayerProjectile::SetDirection(const Vector3D& direction_) noexcept
{
	direction = direction_.GetNormalized();
}

void PlayerProjectile::SetSpeed(float speed_) noexcept
{
	speed = std::max(0.0f, speed_);
}

void PlayerProjectile::SetLifeTime(float lifeTime_) noexcept
{
	lifeTime = std::max(0.0f, lifeTime_);
}

void PlayerProjectile::OnAwake()
{
	direction = direction.GetNormalized();
}

void PlayerProjectile::OnUpdate()
{
	GameObject* owner{ GetOwner() };
	if (owner == nullptr)
	{
		return;
	}

	Transform* transform{ owner->GetComponent<Transform>() };
	if (transform == nullptr)
	{
		return;
	}

	const Vector3D currentPosition{ transform->GetWorldPosition() };
	transform->SetWorldPosition(currentPosition + direction * speed * TimeSystem::GetInstance().GetDeltaTime());

	if ((elapsedTime += TimeSystem::GetInstance().GetDeltaTime()) >= lifeTime)
	{
		Scene* scene{ owner->GetScene() };
		if (scene != nullptr)
		{
			scene->Destroy(owner);
		}
	}
}

void PlayerProjectile::OnCollisionEnter(Collider* other_)
{
	GameObject* const owner{ GetOwner() };
	if (owner == nullptr || other_ == nullptr || other_->GetOwner() == nullptr)
	{
		return;
	}

	GameObject* const otherOwner{ other_->GetOwner() };
	const std::wstring& otherName{ otherOwner->GetName() };
	const std::wstring& otherTag{ otherOwner->GetTag() };

	const bool isWorldGeometry{
		otherTag == L"Wall" || otherTag == L"Floor" || otherTag == L"Stair" ||
		otherName.find(L"Wall") != std::wstring::npos ||
		otherName.find(L"Floor") != std::wstring::npos ||
		otherName.find(L"Stair") != std::wstring::npos
	};

	if (isWorldGeometry || otherTag == L"Enemy")
	{
		if (Scene* const scene{ owner->GetScene() })
		{
			if (otherTag == L"Enemy")
			{
				if (EnemyController* const enemyController{ otherOwner->GetComponent<EnemyController>() }; enemyController != nullptr)
				{
					enemyController->Die();
				}
				else
				{
					scene->Destroy(otherOwner);
				}
			}

			scene->Destroy(owner);
		}
	}
}
