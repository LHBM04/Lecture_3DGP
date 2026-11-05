#pragma once

#include <memory>

#include <wrl/client.h>

#include <directx/d3d12.h>
#include <directxtk12/SpriteBatch.h>

#include "../Entities/Component.hpp"

namespace TUK::Framework
{
	class GPUDevice;
	class ImageAsset;

	class SpriteRenderer : public Component
	{
	public:
		using Component::Component;

		[[nodiscard]] const ImageAsset* GetImage() const noexcept;
		void SetImage(const ImageAsset* value) noexcept;

	private:
		const ImageAsset* image = nullptr;
	};

	inline const ImageAsset* SpriteRenderer::GetImage() const noexcept
	{
		return image;
	}

	inline void SpriteRenderer::SetImage(const ImageAsset* value) noexcept
	{
		image = value;
	}
}
