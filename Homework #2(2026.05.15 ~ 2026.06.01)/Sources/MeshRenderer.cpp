#include "Precompiled.h"
#include "MeshRenderer.h"

#include "Mesh.h"
#include "Material.h"
#include "RenderTarget.h"
#include "GameObject.h"
#include "Transform.h"

void MeshRenderer::SetMesh(Mesh* mesh_) noexcept
{
	mesh = mesh_;
}

Mesh* MeshRenderer::GetMesh() noexcept
{
	return mesh;
}

const Mesh* MeshRenderer::GetMesh() const noexcept
{
	return mesh;
}

void MeshRenderer::SetMaterial(Material* material_) noexcept
{
	material = material_;
}

Material* MeshRenderer::GetMaterial() noexcept
{
	return material;
}

const Material* MeshRenderer::GetMaterial() const noexcept
{
	return material;
}

void MeshRenderer::OnRender(RenderTarget& renderTarget_)
{
	if (nullptr != mesh)
	{
		renderTarget_.SetMaterial(nullptr != material ? *material : Material::GetDefault());
		renderTarget_.SetObject(GetOwner()->GetTransform()->GetWorldMatrix());
		renderTarget_.DrawMesh(*mesh);
	}
}
