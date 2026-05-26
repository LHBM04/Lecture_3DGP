#include "Precompiled.h"
#include "MeshRenderer.h"

#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "RenderContext.h"
#include "Shader.h"
#include "Transform.h"

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

void MeshRenderer::OnRender(RenderContext& context_)
{
	GameObject* owner{ GetOwner() };
	if (nullptr == owner || nullptr == mesh || nullptr == material)
	{
		return;
	}

	Transform* transform{ owner->GetComponent<Transform>() };
	if (nullptr == transform)
	{
		return;
	}

	Shader* shader{ material->GetShader() };
	if (nullptr == shader)
	{
		return;
	}

	context_.UseProgram(shader);
	context_.BindVertexBuffer(mesh->GetVertexBufferView(), mesh->GetVertexCount(), mesh->GetId());
	if (mesh->HasIndexBuffer())
	{
		context_.BindElementBuffer(mesh->GetIndexBufferView(), mesh->GetIndexCount());
	}
	context_.BindMaterial(material);
	context_.SetModelMatrix(transform->GetWorldMatrix());

	if (mesh->HasIndexBuffer())
	{
		context_.DrawElements();
	}
	else
	{
		context_.DrawArrays();
	}
}
