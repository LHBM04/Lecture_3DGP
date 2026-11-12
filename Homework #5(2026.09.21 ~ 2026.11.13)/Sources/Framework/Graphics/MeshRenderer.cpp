#include "Precompiled.hpp"
#include "MeshRenderer.hpp"

#include "../Core/Common.hpp"

#include "../GameObjects/GameObject.hpp"
#include "../GameObjects/Transform.hpp"

#include "../Resources/MaterialAsset.hpp"
#include "../Resources/MeshAsset.hpp"
#include "../Resources/TextureAsset.hpp"

#include "GPUCommandBuffer.hpp"
#include "ShaderConstants.hpp"

namespace TUK::Framework
{
	void MeshRenderer::OnRender(GPUCommandBuffer& commandBuffer)
	{
		if (!mesh || !material)
		{
			return;
		}

		if (material->GetTargetFormat() != commandBuffer.GetActiveTargetFormat())
		{
			LOG_ERROR("메시 머티리얼이 현재 렌더 타깃과 호환되지 않습니다.");
			return;
		}

		const ColorRGBA<f32>& baseColor = material->GetBaseColor();
		const ColorRGBA<f32>& emissiveColor = material->GetEmissiveColor();
		const ColorRGBA<f32>& specularColor = material->GetSpecularColor();
		MaterialConstants constants{};
		constants.baseColor =
		{
			baseColor.GetR() * color.GetR(),
			baseColor.GetG() * color.GetG(),
			baseColor.GetB() * color.GetB(),
			baseColor.GetA() * color.GetA()
		};
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

		if (auto result = commandBuffer.SetGraphicsConstants(constants); !result)
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
		ObjectConstants objectConstants{};
		DirectX::XMStoreFloat4x4(&objectConstants.world, Matrix4x4<f32>::Load(worldMatrix));

		if (auto result = commandBuffer.SetGraphicsConstants(objectConstants); !result)
		{
			LOG_ERROR("메시 변환을 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		const GPUMesh& gpuMesh = mesh->GetGPUMesh();
		commandBuffer.SetGPUGraphicsPipelineState(material->GetGraphicsPipelineState());
		commandBuffer.SetPrimitiveTopology(material->GetPrimitive());
		commandBuffer.SetVertexBuffer(0, gpuMesh.GetD3DVertexBufferView());
		commandBuffer.SetIndexBuffer(gpuMesh.GetD3DIndexBufferView());
		commandBuffer.DrawIndexed(gpuMesh.GetIndexCount());
	}
}
