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
	GameObjectConstants objectConstants{};
	if (const Transform* const transform{ owner->GetTransform() }; transform != nullptr)
	{
		objectConstants.worldMatrix = transform->GetWorldMatrix();
	}
	const D3D12_GPU_VIRTUAL_ADDRESS objectGpuAddress{
		renderSystem.UploadConstantData(&objectConstants, sizeof(objectConstants))
	};
	if (objectGpuAddress != 0)
	{
		commandList_->SetGraphicsRootConstantBufferView(2, objectGpuAddress);
	}

	MaterialConstants materialConstants{};
	materialConstants.albedoColor = material->GetColor();
	const D3D12_GPU_VIRTUAL_ADDRESS materialGpuAddress{
		renderSystem.UploadConstantData(&materialConstants, sizeof(materialConstants))
	};
	if (materialGpuAddress != 0)
	{
		commandList_->SetGraphicsRootConstantBufferView(3, materialGpuAddress);
	}

	material->Bind(commandList_);
	mesh->Bind(commandList_);
	mesh->Draw(commandList_);
}
