#include "Precompiled.h"
#include "ExplodeParticle.h"

#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"

void ExplodeParticle::SetVelocity(const Vector3D& velocity_) noexcept
{
	velocity = velocity_;
}

void ExplodeParticle::SetLifeTime(float lifeTime_) noexcept
{
	lifeTime = std::max(0.0f, lifeTime_);
}

void ExplodeParticle::SetGravity(float gravity_) noexcept
{
	gravity = gravity_;
}

void ExplodeParticle::OnUpdate(float deltaTime_)
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

	velocity.y += gravity * deltaTime_;
	transform->SetWorldPosition(transform->GetWorldPosition() + velocity * deltaTime_);

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
