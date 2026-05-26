#include "Precompiled.h"
#include "MeshRenderer.h"

#include "Application.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "Renderer.h"
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

	Renderer& renderer{ Application::GetRenderer() };

	Transform* transform{ owner->GetComponent<Transform>() };
	if (nullptr == transform)
	{
		return;
	}

	renderer.UseProgram(shader);
	renderer.BindVertexBuffer(mesh->GetVertexBufferView(), mesh->GetVertexCount(), mesh->GetId());
	if (mesh->HasIndexBuffer())
	{
		renderer.BindElementBuffer(mesh->GetIndexBufferView(), mesh->GetIndexCount());
	}
	renderer.BindMaterial(material);
	renderer.SetModelMatrix(transform->GetWorldMatrix());
	if (mesh->HasIndexBuffer())
	{
		renderer.DrawElements();
	}
	else
	{
		renderer.DrawArrays();
	}
}
