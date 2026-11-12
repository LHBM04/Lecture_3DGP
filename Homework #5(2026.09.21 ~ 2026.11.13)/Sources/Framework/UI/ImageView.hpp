#pragma once

#include <expected>
#include <optional>
#include <string>

#include "../Graphics/GPUGraphicsPipelineState.hpp"

#include "../GameObjects/Component.hpp"

namespace TUK::Framework
{
	class TextureAsset;
	class MaterialAsset;

	class ImageView final : public Component
	{
	public:
		using Component::Component;

		void OnPostRender(GPUCommandBuffer& commandBuffer) override;

		[[nodiscard]] const TextureAsset* GetImage() const noexcept;
		void SetImage(const TextureAsset* value) noexcept;

		[[nodiscard]] const MaterialAsset* GetMaterial() const noexcept;
		void SetMaterial(const MaterialAsset* value) noexcept;

	private:
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
}
