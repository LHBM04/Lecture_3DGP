#include "Precompiled.hpp"
#include "MeshRenderer.hpp"

#include "Camera.hpp"
#include "CollisionBox.hpp"
#include "CollisionFrustum.hpp"
#include "CollisionSphere.hpp"
#include "Collider.hpp"
#include "Mesh.hpp"
#include "Object.hpp"
#include "Renderer.hpp"
#include "Transform.hpp"

namespace
{
}

Mesh* MeshRenderer::GetMesh() const
{
	return mesh;
}

void MeshRenderer::SetMesh(Mesh* mesh_)
{
	mesh = mesh_;
}

ColorRGBA MeshRenderer::GetColor() const
{
	return color;
}

void MeshRenderer::SetColor(const ColorRGBA& color_)
{
	color = color_;
}

void MeshRenderer::OnRender()
{
	if (!mesh)
	{
		return;
	}

	const Camera* camera = Renderer::GetCamera();
	const Transform* transform = GetTransform();
	if (!camera || !transform)
	{
		return;
	}

	const Object* cameraOwner = camera->GetOwner();
	const Object* owner = GetOwner();
	if (cameraOwner && owner)
	{
		const CollisionFrustum* frustum = cameraOwner->GetComponent<CollisionFrustum>();
		if (frustum)
		{
			const Collider* objectCollider = owner->GetComponent<CollisionSphere>();
			if (!objectCollider)
			{
				objectCollider = owner->GetComponent<CollisionBox>();
			}
			if (objectCollider && !frustum->Intersects(*objectCollider))
			{
				return;
			}
		}
	}

	const auto& vertices = mesh->GetVertices();
	if (vertices.empty())
	{
		return;
	}

	const int screenWidth = std::max(Renderer::GetWidth(), 1);
	const int screenHeight = std::max(Renderer::GetHeight(), 1);
	if (screenWidth <= 0 || screenHeight <= 0)
	{
		return;
	}

	Renderer::QueueMesh(mesh, color);
}
