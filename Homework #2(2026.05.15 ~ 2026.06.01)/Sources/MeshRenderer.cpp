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
	ID3D12Device* const device{ rs.GetDevice() };
	if (device == nullptr)
	{
		return;
	}

	if (!mesh->HasGpuBuffers())
	{
		if (!mesh->CreateBuffers(device))
		{
			return;
		}
	}

	const Transform* const transform{ GetOwner()->GetComponent<Transform>() };
	if (transform == nullptr)
	{
		return;
	}

	const Matrix4x4& worldMatrix{ transform->GetWorldMatrix() };
	
	RenderSystem::ObjectConstants objectData{};
	objectData.worldMatrix = worldMatrix;
	rs.SetObjectConstants(objectData);

	RenderSystem::MaterialConstants materialData{};
	materialData.color = material->GetColor();
	rs.SetMaterialConstants(materialData);

	if (rs.GetCommandList() == nullptr)
	{
		return;
	}

	rs.GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	rs.SetVertexBuffer(mesh->GetVertexBufferView());

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
