#pragma once

#include <wrl/client.h>

#include "../Math/ColorRGBA.hpp"

#include "../GameObjects/Component.hpp"

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

		[[nodiscard]] const ColorRGBA<f32>& GetColor() const noexcept;
		void SetColor(const ColorRGBA<f32>& value) noexcept;

	private:
		ColorRGBA<f32> color = ColorRGBA<f32>::GetWhite();
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

	inline const ColorRGBA<f32>& SpriteRenderer::GetColor() const noexcept
	{
		return color;
	}

	inline void SpriteRenderer::SetColor(const ColorRGBA<f32>& value) noexcept
	{
		color = value;
	}
}
