#include "Precompiled.hpp"
#include "SkyboxRenderer.hpp"

#include "../Core/Common.hpp"

#include "../Resources/CubemapAsset.hpp"
#include "../Resources/MaterialAsset.hpp"

#include "GPUCommandBuffer.hpp"
#include "GPUConstants.hpp"

namespace TUK::Framework
{
	void SkyboxRenderer::OnRender(GPUCommandBuffer& commandBuffer)
	{
		if (!cubemap || !material)
		{
			return;
		}

		const SkyboxConstants constants
		{
			.color = color
		};
		if (auto result = commandBuffer.SetGraphicsConstants(constants); !result)
		{
			LOG_ERROR("스카이박스 색상 상수를 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		const GPUTexture2D& texture = cubemap->GetTexture();
		const auto description = texture.GetD3DResource().GetDesc();

		D3D12_SHADER_RESOURCE_VIEW_DESC view{};
		view.Format = description.Format;
		view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		view.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		view.TextureCube.MipLevels = description.MipLevels;

		const std::array<const GPUTexture2D*, GPUCommandBuffer::TEXTURE_SLOT_COUNT> textures{ &texture };
		if (auto bound = commandBuffer.BindTextures(textures, &view); !bound)
		{
			LOG_ERROR("스카이박스 텍스처를 바인딩하지 못했습니다: {}", bound.error());
			return;
		}

		commandBuffer.SetGPUGraphicsPipelineState(material->GetGraphicsPipelineState());
		commandBuffer.SetPrimitiveTopology(material->GetPrimitive());

		constexpr u32 FULLSCREEN_VERTEX_COUNT = 3;
		commandBuffer.Draw(FULLSCREEN_VERTEX_COUNT);
	}
}
