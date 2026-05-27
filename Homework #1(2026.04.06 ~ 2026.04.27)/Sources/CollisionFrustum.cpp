#include "Precompiled.hpp"
#include "CollisionFrustum.hpp"

#include "Camera.hpp"
#include "Object.hpp"
#include "OrthographicsCamera.hpp"
#include "PerspectiveCamera.hpp"
#include "Transform.hpp"

const DirectX::BoundingFrustum& CollisionFrustum::GetBounds() const
{
	return bounds;
}

DirectX::BoundingBox CollisionFrustum::GetWorldAABB() const
{
	DirectX::XMFLOAT3 corners[DirectX::BoundingFrustum::CORNER_COUNT]{};
	bounds.GetCorners(corners);

	DirectX::BoundingBox aabb{};
	DirectX::BoundingBox::CreateFromPoints(aabb, DirectX::BoundingFrustum::CORNER_COUNT, corners, sizeof(DirectX::XMFLOAT3));
	return aabb;
}

void CollisionFrustum::UpdateBounds()
{
	Object* owner = GetOwner();
	Transform* transform = GetTransform();
	if (!owner || !transform)
	{
		bounds = DirectX::BoundingFrustum();
		return;
	}

	const Camera* camera = owner->GetComponent<PerspectiveCamera>();
	if (!camera)
	{
		camera = owner->GetComponent<OrthographicsCamera>();
	}
	if (!camera)
	{
		bounds = DirectX::BoundingFrustum();
		return;
	}

	const Camera::Viewport viewport = camera->GetViewport();
	const float aspectRatio = std::max(viewport.width, Mathf::EPSILON) / std::max(viewport.height, Mathf::EPSILON);
	const Matrix4x4 projectionMatrix = camera->GetProjectionMatrix(aspectRatio);

	DirectX::BoundingFrustum localFrustum{};
	DirectX::BoundingFrustum::CreateFromMatrix(localFrustum, Matrix4x4::Load(projectionMatrix));
	localFrustum.Transform(bounds, Matrix4x4::Load(transform->GetWorldMatrix()));
}
