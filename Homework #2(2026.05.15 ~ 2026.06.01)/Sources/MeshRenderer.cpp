#include "Precompiled.h"

#include "MeshRenderer.h"

#include "GameObject.h"
#include "Logger.h"
#include "Material.h"
#include "Mesh.h"
#include "RenderSystem.h"
#include "Scene.h"
#include "Transform.h"

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
	material = material_;
}

void MeshRenderer::OnRender()
{
	if (mesh == nullptr)
	{
		Logger::Warning(L"No Mesh!");
		return;
	}

	RenderSystem& rs{ RenderSystem::GetInstance() };
	if (!mesh->HasGpuBuffers())
	{
		if (!mesh->CreateBuffers(rs.GetDevice()))
		{
			Logger::Warning(L"Mesh GPU buffer upload failed.");
			return;
		}
	}

	// 1. Set Constants
	RenderSystem::ObjectConstants objData{ GetOwner()->GetComponent<Transform>()->GetWorldMatrix() };
	rs.SetObjectConstants(objData);

	if (material != nullptr)
	{
		RenderSystem::MaterialConstants matData{ material->GetBaseColor() };
		rs.SetMaterialConstants(matData);
	}

	// 2. Set Buffers
	rs.SetVertexBuffer(mesh->GetVertexBufferView());
	rs.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 3. Draw
	if (mesh->GetIndexCount() > 0)
	{
		rs.SetIndexBuffer(mesh->GetIndexBufferView());
		rs.DrawIndexedInstanced(mesh->GetIndexCount(), 1, 0, 0, 0);
	}
	else
	{
		rs.DrawInstanced(static_cast<UINT>(mesh->GetVertices().size()), 1, 0, 0);
	}
}
