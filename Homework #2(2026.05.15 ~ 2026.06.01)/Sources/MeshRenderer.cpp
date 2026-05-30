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
	if (mesh == nullptr)
	{
		Logger::Warning(L"No Mesh!");
		return;
	}

	auto& rs = RenderSystem::GetInstance();

	// 1. Set Constants
	RenderSystem::ObjectConstants objData{ GetOwner()->GetComponent<Transform>()->GetWorldMatrix() };
	rs.SetObjectConstants(objData);

	if (material)
	{
		RenderSystem::MaterialConstants matData{ material->GetBaseColor(), material->GetRoughness(), material->GetMetallic(), { 0, 0 } };
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
