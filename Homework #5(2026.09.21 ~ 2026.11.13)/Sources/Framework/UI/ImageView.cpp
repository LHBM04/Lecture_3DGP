#include "Precompiled.hpp"
#include "ImageView.hpp"

#include "../Core/Common.hpp"

#include "../Graphics/GPUCommandBuffer.hpp"

namespace TUK::Framework
{
	void ImageView::OnPostRender(GPUCommandBuffer& commandBuffer)
	{
		if (!image)
		{
			return;
		}

		if (auto result = commandBuffer.DrawImageUI(GetOwner(), image); !result)
		{
			LOG_ERROR("UI 이미지 렌더링에 실패했습니다: {}", result.error());
		}
	}
}
