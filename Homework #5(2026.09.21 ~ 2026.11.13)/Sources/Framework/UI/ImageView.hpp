#pragma once

#include "../Graphics/GPUGraphicsPipelineState.hpp"

#include "../Math/ColorRGBA.hpp"

#include "../GameObjects/Component.hpp"

namespace TUK::Framework
{
	class TextureAsset;
	class MaterialAsset;

	class ImageView final : public Component
	{
		using Component::Component;

	public:
		void OnPostRender(GPUCommandBuffer& commandBuffer) override;

		[[nodiscard]] const TextureAsset* GetImage() const noexcept;
		void SetImage(const TextureAsset* value) noexcept;

		[[nodiscard]] const MaterialAsset* GetMaterial() const noexcept;
		void SetMaterial(const MaterialAsset* value) noexcept;

		[[nodiscard]] const ColorRGBA<f32>& GetColor() const noexcept;
		void SetColor(const ColorRGBA<f32>& value) noexcept;

	private:
		ColorRGBA<f32> color = ColorRGBA<f32>::GetWhite();
		const TextureAsset* texture = nullptr;
		const MaterialAsset* material = nullptr;
	};

	inline const TextureAsset* ImageView::GetImage() const noexcept
	{
		return texture;
	}

	inline void ImageView::SetImage(const TextureAsset* value) noexcept
	{
		texture = value;
	}

	inline const MaterialAsset* ImageView::GetMaterial() const noexcept
	{
		return material;
	}

	inline void ImageView::SetMaterial(const MaterialAsset* value) noexcept
	{
		material = value;
	}

	inline const ColorRGBA<f32>& ImageView::GetColor() const noexcept
	{
		return color;
	}

	inline void ImageView::SetColor(const ColorRGBA<f32>& value) noexcept
	{
		color = value;
	}
}
