#pragma once

#include "../Entities/Component.hpp"

namespace TUK::Framework
{
	class TextureAsset;

	class ImageView final : public Component
	{
	public:
		using Component::Component;

		void OnPostRender(GPUCommandBuffer& commandBuffer) override;

		[[nodiscard]] const TextureAsset* GetImage() const noexcept;
		void SetImage(const TextureAsset* value) noexcept;

	private:
		const TextureAsset* image = nullptr;
	};

	inline const TextureAsset* ImageView::GetImage() const noexcept
	{
		return image;
	}

	inline void ImageView::SetImage(const TextureAsset* value) noexcept
	{
		image = value;
	}
}
