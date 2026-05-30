#include "Precompiled.h"
#include "CubeCollider.h"
#include "Transform.h"
#include "GameObject.h"

void CubeCollider::OnUpdate(float deltaTime_)
{
	auto* transform = GetOwner()->GetComponent<Transform>();
	if (!transform)
	{
		return;
	}

	localBox.Transform(worldBox, Matrix4x4::Load(transform->GetWorldMatrix()));
}

bool CubeCollider::IsIntersects(const DirectX::BoundingFrustum& frustum_) const
{
	return frustum_.Intersects(worldBox);
}

