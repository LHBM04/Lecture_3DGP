#include "Precompiled.hpp"
#include "Particle.hpp"

#include "MeshRenderer.hpp"
#include "Object.hpp"
#include "Timer.hpp"
#include "Transform.hpp"

const Vector3D& Particle::GetVelocity() const
{
	return velocity;
}

void Particle::SetVelocity(const Vector3D& velocity_)
{
	velocity = velocity_;
}

float Particle::GetLifetime() const
{
	return lifetime;
}

void Particle::SetLifetime(float lifetime_)
{
	lifetime = std::max(lifetime_, 0.01f);
}

void Particle::OnAttach()
{
	const Transform* transform = GetTransform();
	initialScale = transform ? transform->GetWorldScale() : Vector3D::GetOne();
}

void Particle::OnUpdate()
{
	Transform* transform = GetTransform();
	Object* owner = GetOwner();
	if (!transform || !owner)
	{
		return;
	}

	const float deltaTime = Timer::GetDeltaTime();
	elapsedTime += deltaTime;

	velocity += Vector3D(0.0f, -9.8f, 0.0f) * (deltaTime * 0.35f);
	transform->SetWorldPosition(transform->GetWorldPosition() + velocity * deltaTime);

	const float t = Mathf::Clamp(elapsedTime / lifetime, 0.0f, 1.0f);
	const float scaleFactor = 1.0f - t;
	transform->SetWorldScale(initialScale * scaleFactor);

	MeshRenderer* meshRenderer = owner->GetComponent<MeshRenderer>();
	if (meshRenderer)
	{
		ColorRGBA color = meshRenderer->GetColor();
		color.w = 1.0f - t;
		meshRenderer->SetColor(color);
	}

	if (elapsedTime >= lifetime)
	{
		owner->Destroy();
	}
}
