#pragma once

#include <memory>

#include <wrl/client.h>

#include <directx/d3d12.h>
#include <directxtk12/SpriteBatch.h>

#include "../Entities/Component.hpp"

namespace TUK::Framework
{
	class GPUDevice;
	class TextureAsset;

	class SpriteRenderer : public Component
	{
	public:
		using Component::Component;

		[[nodiscard]] const TextureAsset* GetImage() const noexcept;
		void SetImage(const TextureAsset* value) noexcept;

	private:
		const TextureAsset* image = nullptr;
	};

	inline const TextureAsset* SpriteRenderer::GetImage() const noexcept
	{
		return image;
	}

	inline void SpriteRenderer::SetImage(const TextureAsset* value) noexcept
	{
		image = value;
	}
}
