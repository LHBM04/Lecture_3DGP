#include "Precompiled.hpp"
#include "MeshRenderer.hpp"

#include "../Core/Common.hpp"

#include "../Objects/GameObject.hpp"
#include "../Objects/Terrain.hpp"
#include "../Objects/Transform.hpp"

#include "../Resources/MaterialAsset.hpp"
#include "../Resources/MeshAsset.hpp"
#include "../Resources/TerrainAsset.hpp"
#include "../Resources/TextureAsset.hpp"

#include "GPUCommandBuffer.hpp"
#include "GPUConstants.hpp"
#include "GPUMesh.hpp"

namespace TUK::Framework
{
	namespace
	{
		void DrawMesh(
			GPUCommandBuffer& commandBuffer,
			const GPUMesh& gpuMesh,
			const MaterialAsset& material,
			const ColorRGBA<f32>& color,
			const Matrix4x4<f32>& worldMatrix)
		{
			if (material.GetTargetFormat() != commandBuffer.GetActiveTargetFormat())
			{
				LOG_ERROR("메시 머티리얼이 현재 렌더 타깃과 호환되지 않습니다.");
				return;
			}

			MaterialConstants constants
			{
				.baseColor = material.GetBaseColor() * color,
				.emissiveColor = material.GetEmissiveColor(),
				.metallic = material.GetMetallic(),
				.roughness = material.GetRoughness(),
				.specularColor = material.GetSpecularColor()
			};

			static_assert(GPUCommandBuffer::TEXTURE_SLOT_COUNT == static_cast<u32>(MaterialTextureSlot::COUNT));
			std::array<const GPUTexture2D*, GPUCommandBuffer::TEXTURE_SLOT_COUNT> textures{};
			const MaterialTextures& materialTextures = material.GetTextures();
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

			const ObjectConstants objectConstants
			{
				.world = worldMatrix
			};

			if (auto result = commandBuffer.SetGraphicsConstants(objectConstants); !result)
			{
				LOG_ERROR("메시 변환을 바인딩하지 못했습니다: {}", result.error());
				return;
			}

			commandBuffer.SetGPUGraphicsPipelineState(material.GetGraphicsPipelineState());
			commandBuffer.SetPrimitiveTopology(material.GetPrimitive());
			commandBuffer.SetVertexBuffer(0, gpuMesh.GetD3DVertexBufferView());
			commandBuffer.SetIndexBuffer(gpuMesh.GetD3DIndexBufferView());
			commandBuffer.DrawIndexed(gpuMesh.GetIndexCount());
		}
	}

	void MeshRenderer::OnRender(GPUCommandBuffer& commandBuffer)
	{
		if (!material)
		{
			return;
		}

		const GameObject* owner = GetOwner();
		const GPUMesh* gpuMesh = nullptr;
		if (mesh)
		{
			gpuMesh = &mesh->GetGPUMesh();
		}
		else if (const Terrain* terrain = owner ? owner->GetComponent<Terrain>() : nullptr; terrain && terrain->IsValid())
		{
			gpuMesh = &terrain->GetTerrain()->GetGPUMesh();
		}

		if (!gpuMesh)
		{
			return;
		}

		const Transform* transform = owner ? owner->GetComponent<Transform>() : nullptr;
		const Matrix4x4<f32>& worldMatrix = transform ? transform->GetWorldMatrix() : Matrix4x4<f32>::GetIdentity();
		DrawMesh(commandBuffer, *gpuMesh, *material, color, worldMatrix);
	}
}
