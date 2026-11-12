#include "Precompiled.hpp"
#include "ImageView.hpp"

#include "../Core/Common.hpp"

#include "../Objects/GameObject.hpp"
#include "../Objects/Transform.hpp"

#include "../Graphics/GPUCommandBuffer.hpp"
#include "../Graphics/GPUConstants.hpp"
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

		const ObjectConstants objectConstants
		{
			.world = transform->GetWorldMatrix()
		};

		if (auto result = commandBuffer.SetGraphicsConstants(objectConstants); !result)
		{
			LOG_ERROR("UI 이미지 변환을 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		const ImageUIConstants constants
		{
			.viewportSize = Vector2D<f32>{ 1280.0f, 960.0f },
			.color = color
		};
		if (auto result = commandBuffer.SetGraphicsConstants(constants); !result)
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
