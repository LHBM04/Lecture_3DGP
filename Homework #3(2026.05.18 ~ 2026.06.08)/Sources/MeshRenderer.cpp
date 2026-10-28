#include "Precompiled.h"

#include "MeshRenderer.h"

#include "Camera.h"
#include "GameObject.h"
#include "Logger.h"
#include "Material.h"
#include "Mesh.h"
#include "RenderSystem.h"
#include "Scene.h"
#include "Transform.h"

void MeshRenderer::OnRender()
{
	if (mesh == nullptr || material == nullptr)
	{
		return;
	}

	const Transform* const transform{ GetOwner()->GetComponent<Transform>() };
	if (transform == nullptr)
	{
		return;
	}

	const Matrix4x4& worldMatrix{ transform->GetWorldMatrix() };
	Scene* const scene{ GetOwner()->GetScene() };
	if (scene != nullptr)
	{
		const std::span<Camera* const> cameras{ scene->GetCameras() };
		Camera* const camera{ cameras.empty() ? nullptr : cameras.front() };
		if (camera != nullptr)
		{
			DirectX::BoundingBox bounds{};
			bounds.Center = mesh->GetBoundsCenter();
			bounds.Extents = mesh->GetBoundsExtents();

			if (!camera->IsInFrustum(bounds, worldMatrix))
			{
				return;
			}
		}

		RenderSystem::GetInstance().SubmitRenderRequest(mesh, material, worldMatrix);
	}
}

Mesh* MeshRenderer::GetMesh() const noexcept
{
	return mesh;
}

void MeshRenderer::SetMesh(Mesh* mesh_) noexcept
{
	mesh = mesh_;
}

Material* MeshRenderer::GetMaterial() const noexcept
{
	assert(material != nullptr && "MeshRenderer must have a material!");
	return material;
}

void MeshRenderer::SetMaterial(Material* material_) noexcept
{
	if (material_ == nullptr)
	{
		Logger::Critical(L"Material is null.");
	}

	material = material_;
}
