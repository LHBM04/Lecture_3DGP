#include "Precompiled.h"
#include "SphereCollider.h"
#include "Transform.h"
#include "GameObject.h"

void SphereCollider::OnUpdate(float deltaTime_) noexcept
{
	auto* transform = GetOwner()->GetComponent<Transform>();
	if (!transform)
	{
		return;
	}

	localSphere.Transform(worldSphere, Matrix4x4::Load(transform->GetWorldMatrix()));
}

bool SphereCollider::IsIntersects(const DirectX::BoundingFrustum& frustum_) const noexcept
{
	return frustum_.Intersects(worldSphere);
}
