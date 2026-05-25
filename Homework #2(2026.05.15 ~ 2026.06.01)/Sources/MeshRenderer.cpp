#include "Precompiled.h"
#include "MeshRenderer.h"

#include "Application.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Shader.h"
#include "Transform.h"
#include "DrawCall.h"

namespace
{
	uint64_t BuildSortKey(uint64_t pipelineId_, uint64_t materialId_, uint64_t meshId_) noexcept
	{
		return ((pipelineId_ & 0xFFFFull) << 48) |
			((materialId_ & 0xFFFFFFull) << 24) |
			(meshId_ & 0xFFFFFFull);
	}
}

Mesh* MeshRenderer::GetMesh() noexcept
{
	return mesh;
}

const Mesh* MeshRenderer::GetMesh() const noexcept
{
	return mesh;
}

void MeshRenderer::SetMesh(Mesh* mesh_) noexcept
{
	mesh = mesh_;
}

Material* MeshRenderer::GetMaterial() noexcept
{
	return material;
}

const Material* MeshRenderer::GetMaterial() const noexcept
{
	return material;
}

void MeshRenderer::SetMaterial(Material* material_) noexcept
{
	material = material_;
}

void MeshRenderer::OnRender()
{
	GameObject* owner{ GetOwner() };

	if (nullptr == mesh || nullptr == material)
	{
		return;
	}

	Shader* shader{ material->GetShader() };
	if (nullptr == shader)
	{
		return;
	}

	Renderer& renderer{ Application::GetRenderer()};

	Transform* transform{ owner->GetComponent<Transform>() };
	if (nullptr == transform)
	{
		return;
	}

	DrawCall drawCall{};
	drawCall.pipelineState = shader->GetPipelineState();
	drawCall.graphicsRootSignature = shader->GetGraphicsRootSignature();
	drawCall.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	drawCall.vertexBufferView = mesh->GetVertexBufferView();
	drawCall.indexBufferView = mesh->GetIndexBufferView();
	drawCall.hasIndexBuffer = mesh->HasIndexBuffer();
	drawCall.indexed = mesh->HasIndexBuffer();
	drawCall.materialDescriptorTable = material->GetDescriptorTable();
	drawCall.materialColor = material->GetColor();
	drawCall.vertexCount = mesh->GetVertexCount();
	drawCall.startVertexLocation = 0;
	drawCall.indexCount = mesh->GetIndexCount();
	drawCall.startIndexLocation = 0;
	drawCall.baseVertexLocation = 0;
	drawCall.instanceCount = 1;
	drawCall.startInstanceLocation = 0;
	drawCall.pipelineId = shader->GetPipelineId();
	drawCall.materialId = material->GetId();
	drawCall.meshId = mesh->GetId();
	drawCall.worldTransform = transform->GetWorldMatrix();
	drawCall.sortKey = BuildSortKey(drawCall.pipelineId, drawCall.materialId, drawCall.meshId);

	renderer.SubmitDrawCall(drawCall);
}
