#include "Precompiled.hpp"
#include "MeshRenderer.hpp"

#include <array>
#include <span>

#include "../Core/Common.hpp"

#include "../Objects/GameObject.hpp"
#include "../Objects/Transform.hpp"

#include "../Resources/MaterialAsset.hpp"
#include "../Resources/MeshAsset.hpp"
#include "../Resources/TextureAsset.hpp"

#include "GPUCommandBuffer.hpp"

namespace TUK::Framework
{
	void MeshRenderer::OnRender(GPUCommandBuffer& commandBuffer)
	{
		if (!mesh || !material)
		{
			return;
		}

		if (material->GetPass() != MaterialPass::SURFACE ||
			material->GetTargetFormat() != commandBuffer.GetActiveTargetFormat())
		{
			LOG_ERROR("메시 머티리얼이 현재 렌더 타깃과 호환되지 않습니다.");
			return;
		}

		struct MaterialConstants final
		{
			DirectX::XMFLOAT4 baseColor;
			DirectX::XMFLOAT4 emissiveColor;
			f32 metallic;
			f32 roughness;
			u32 textureMask;
			f32 padding{};
			DirectX::XMFLOAT4 specularColor;
		};
		static_assert(sizeof(MaterialConstants) == 64);

		const ColorRGBA<f32>& baseColor = material->GetBaseColor();
		const ColorRGBA<f32>& emissiveColor = material->GetEmissiveColor();
		const ColorRGBA<f32>& specularColor = material->GetSpecularColor();
		MaterialConstants constants{};
		constants.baseColor = { baseColor.GetR(), baseColor.GetG(), baseColor.GetB(), baseColor.GetA() };
		constants.emissiveColor = { emissiveColor.GetR(), emissiveColor.GetG(), emissiveColor.GetB(), emissiveColor.GetA() };
		constants.metallic = material->GetMetallic();
		constants.roughness = material->GetRoughness();
		constants.specularColor = { specularColor.GetR(), specularColor.GetG(), specularColor.GetB(), specularColor.GetA() };

		static_assert(GPUCommandBuffer::TEXTURE_SLOT_COUNT == static_cast<u32>(MaterialTextureSlot::COUNT));
		std::array<const GPUTexture2D*, GPUCommandBuffer::TEXTURE_SLOT_COUNT> textures{};
		const MaterialTextures& materialTextures = material->GetTextures();
		for (u32 index = 0; index < textures.size(); ++index)
		{
			if (const TextureAsset* texture = materialTextures[index])
			{
				textures[index] = &texture->GetTexture();
				constants.textureMask |= 1u << index;
			}
		}

		constexpr u32 MATERIAL_ROOT_SLOT = 2;
		if (auto result = commandBuffer.SetGraphicsConstants(MATERIAL_ROOT_SLOT, std::as_bytes(std::span(&constants, 1))); !result)
		{
			LOG_ERROR("메시 머티리얼 상수를 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		if (auto result = commandBuffer.BindTextures(textures); !result)
		{
			LOG_ERROR("메시 텍스처를 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		const Transform* transform = GetOwner()->GetComponent<Transform>();
		const Matrix4x4<f32>& worldMatrix = transform ? transform->GetWorldMatrix() : Matrix4x4<f32>::GetIdentity();
		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&world, Matrix4x4<f32>::Load(worldMatrix));

		constexpr u32 OBJECT_ROOT_SLOT = 1;
		if (auto result = commandBuffer.SetGraphicsConstants(OBJECT_ROOT_SLOT, std::as_bytes(std::span(&world, 1))); !result)
		{
			LOG_ERROR("메시 변환을 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		const GPUMesh& gpuMesh = mesh->GetGPUMesh();
		commandBuffer.SetGPUGraphicsPipelineState(material->GetGraphicsPipelineState());
		commandBuffer.SetPrimitiveTopology(gpuMesh.GetD3DTopology());
		commandBuffer.SetVertexBuffer(0, gpuMesh.GetD3DVertexBufferView());
		commandBuffer.SetIndexBuffer(gpuMesh.GetD3DIndexBufferView());
		commandBuffer.DrawIndexed(gpuMesh.GetIndexCount());
	}
}
