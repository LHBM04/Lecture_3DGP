#include "Precompiled.h"

#include "TerrainRenderer.h"

#include "Camera.h"
#include "GameObject.h"
#include "Logger.h"
#include "Material.h"
#include "Mesh.h"
#include "RenderSystem.h"
#include "Scene.h"
#include "Terrain.h"
#include "Transform.h"

void TerrainRenderer::OnRender()
{
	if (terrain == nullptr || material == nullptr)
	{
		return;
	}

	Mesh* const mesh{ terrain->GetMesh() };
	if (mesh == nullptr)
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

Terrain* TerrainRenderer::GetTerrain() const noexcept
{
	return terrain;
}

void TerrainRenderer::SetTerrain(Terrain* terrain_) noexcept
{
	terrain = terrain_;
}

Material* TerrainRenderer::GetMaterial() const noexcept
{
	assert(material != nullptr && "TerrainRenderer must have a material!");
	return material;
}

void TerrainRenderer::SetMaterial(Material* material_) noexcept
{
	if (material_ == nullptr)
	{
		Logger::Critical(L"TerrainRenderer material is null.");
	}

	material = material_;
}
