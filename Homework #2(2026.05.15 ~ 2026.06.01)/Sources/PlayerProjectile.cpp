#include "Precompiled.h"
#include "PlayerProjectile.h"

#include "GameObject.h"
#include "Scene.h"
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

void PlayerProjectile::OnUpdate(float deltaTime_)
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
	transform->SetWorldPosition(currentPosition + direction * speed * deltaTime_);

	elapsedTime += deltaTime_;
	if (elapsedTime >= lifeTime)
	{
		Scene* scene{ owner->GetScene() };
		if (scene != nullptr)
		{
			scene->Destroy(owner);
		}
	}
}
