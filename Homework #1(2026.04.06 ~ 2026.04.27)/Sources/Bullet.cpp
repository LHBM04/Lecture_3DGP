#include "Precompiled.hpp"
#include "Bullet.hpp"

#include "Object.hpp"
#include "Quaternion.hpp"
#include "Timer.hpp"
#include "Transform.hpp"

void Bullet::OnUpdate()
{
	Transform* transform = GetTransform();
	if (!transform)
	{
		return;
	}

	Vector3D currentPosition = transform->GetWorldPosition();
	Vector3D forward = transform->GetWorldRotation() * Vector3D::GetForward();
	if (forward.IsZero())
	{
		forward = Vector3D::GetForward();
	}
	forward.Normalize();
	transform->SetWorldPosition(currentPosition + forward * (speed * Timer::GetDeltaTime()));

	lifetime -= Timer::GetDeltaTime();
	if (lifetime <= 0.0f)
	{
		Object* owner = GetOwner();
		if (owner)
		{
			owner->Destroy();
		}
		else
		{
			Destroy();
		}
	}
}
