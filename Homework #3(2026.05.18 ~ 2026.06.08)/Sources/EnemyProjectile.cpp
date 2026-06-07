#include "Precompiled.h"
#include "EnemyProjectile.h"

#include "Collider.h"
#include "GameObject.h"
#include "Scene.h"
#include "TimeSystem.h"
#include "Transform.h"

void EnemyProjectile::SetDirection(const Vector3D& direction_) noexcept
{
	direction = direction_.GetNormalized();
}

void EnemyProjectile::SetSpeed(float speed_) noexcept
{
	speed = std::max(0.0f, speed_);
}

void EnemyProjectile::SetLifeTime(float lifeTime_) noexcept
{
	lifeTime = std::max(0.0f, lifeTime_);
}

void EnemyProjectile::OnAwake()
{
	direction = direction.GetNormalized();
}

void EnemyProjectile::OnUpdate()
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

	transform->SetWorldPosition(
		transform->GetWorldPosition() + direction * speed * TimeSystem::GetInstance().GetDeltaTime());

	if ((elapsedTime += TimeSystem::GetInstance().GetDeltaTime()) >= lifeTime)
	{
		if (Scene* const scene{ owner->GetScene() })
		{
			scene->Destroy(owner);
		}
	}
}

void EnemyProjectile::OnCollisionEnter(Collider* other_)
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

	if (isWorldGeometry || otherTag == L"Player")
	{
		if (Scene* const scene{ owner->GetScene() })
		{
			scene->Destroy(owner);
		}
	}
}
