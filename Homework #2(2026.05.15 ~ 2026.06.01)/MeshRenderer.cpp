#include "Precompiled.h"
#include "MeshRenderer.h"

#include "Mesh.h"
#include "Renderer.h"
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

void MeshRenderer::OnRender(Renderer& renderer_)
{
	if (nullptr != mesh)
	{
		renderer_.DrawMesh(*mesh, GetTransform()->GetWorldMatrix());
	}
}
