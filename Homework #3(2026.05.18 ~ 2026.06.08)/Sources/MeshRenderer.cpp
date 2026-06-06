#include "Precompiled.h"

#include "MeshRenderer.h"

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

	RenderSystem& rs{ RenderSystem::GetInstance() };
	const Transform* const transform{ GetOwner()->GetComponent<Transform>() };
	if (transform == nullptr)
	{
		return;
	}

	const Matrix4x4& worldMatrix{ transform->GetWorldMatrix() };
	
	ObjectConstants objectData{};
	objectData.worldMatrix = worldMatrix;
	rs.SetObjectConstants(objectData);

	MaterialConstants materialData{};
	materialData.baseColor = material->GetBaseColor();
	materialData.emissiveColor = material->GetEmissiveColor();
	materialData.metallic = material->GetMetallic();
	materialData.roughness = material->GetRoughness();
	rs.SetMaterialConstants(materialData);

	rs.DrawMesh(mesh, material);
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
