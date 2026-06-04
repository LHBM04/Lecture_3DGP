#include "Precompiled.h"
#include "MeshRenderer.h"

#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "RenderSystem.h"

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

void MeshRenderer::OnRender(ID3D12GraphicsCommandList* commandList_)
{
	if (commandList_ == nullptr || mesh == nullptr || material == nullptr)
	{
		return;
	}

	ID3D12Device* const device{ RenderSystem::GetInstance().GetDevice() };
	if (device == nullptr)
	{
		return;
	}

	if (!mesh->HasGpuBuffers() && !mesh->CreateBuffers(device))
	{
		return;
	}

	if (!material->IsLoaded() && !material->Load())
	{
		return;
	}

	GameObject* const owner{ GetOwner() };
	if (owner == nullptr)
	{
		return;
	}

	RenderSystem& renderSystem{ RenderSystem::GetInstance() };
	renderSystem.SetObjectConstants(owner->GetGameObjectConstants());
	renderSystem.SetMaterialConstants(material->GetMaterialConstants());
	material->Bind(commandList_);
	mesh->Bind(commandList_);
	mesh->Draw(commandList_);
}
