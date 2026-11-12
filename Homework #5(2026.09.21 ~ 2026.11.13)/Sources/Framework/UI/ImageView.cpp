#include "Precompiled.hpp"
#include "ImageView.hpp"

#include <array>
#include <span>
#include <utility>

#include <directx/d3dx12.h>

#include "../Core/Common.hpp"

#include "../Objects/GameObject.hpp"
#include "../Objects/Transform.hpp"

#include "../Graphics/GPUCommandBuffer.hpp"
#include "../Graphics/GPUDevice.hpp"
#include "../Graphics/GPUTarget.hpp"

#include "../Resources/MaterialAsset.hpp"
#include "../Resources/ShaderAsset.hpp"
#include "../Resources/TextureAsset.hpp"

namespace TUK::Framework
{
	void ImageView::OnPostRender(GPUCommandBuffer& commandBuffer)
	{
		if (!texture || !material)
		{
			return;
		}

		const Transform* transform = GetOwner()->GetComponent<Transform>();
		if (!transform)
		{
			LOG_ERROR("UI 이미지 엔티티에 Transform 컴포넌트가 필요합니다.");
			return;
		}

		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&world, Matrix4x4<f32>::Load(transform->GetWorldMatrix()));

		constexpr u32 OBJECT_ROOT_SLOT = 1;
		if (auto result = commandBuffer.SetGraphicsConstants(OBJECT_ROOT_SLOT, std::as_bytes(std::span(&world, 1))); !result)
		{
			LOG_ERROR("UI 이미지 변환을 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		struct ImageUIConstants final
		{
			DirectX::XMFLOAT2 viewportSize;
			DirectX::XMFLOAT2 padding{};
		};
		static_assert(sizeof(ImageUIConstants) == 16);

		const ImageUIConstants constants{ { 1280, 960 } };
		constexpr u32 IMAGE_UI_ROOT_SLOT = 2;
		if (auto result = commandBuffer.SetGraphicsConstants(IMAGE_UI_ROOT_SLOT, std::as_bytes(std::span(&constants, 1))); !result)
		{
			LOG_ERROR("UI 뷰포트 상수를 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		const std::array<const GPUTexture2D*, GPUCommandBuffer::TEXTURE_SLOT_COUNT> textures{ &texture->GetTexture() };
		if (auto result = commandBuffer.BindTextures(textures); !result)
		{
			LOG_ERROR("UI 이미지 텍스처를 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		commandBuffer.SetGPUGraphicsPipelineState(material->GetGraphicsPipelineState());
		commandBuffer.SetPrimitiveTopology(material->GetPrimitive());
		constexpr u32 IMAGE_VERTEX_COUNT = 4;
		commandBuffer.Draw(IMAGE_VERTEX_COUNT);
	}
}
