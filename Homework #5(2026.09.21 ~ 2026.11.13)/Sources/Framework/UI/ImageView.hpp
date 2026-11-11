#pragma once

#include "../Objects/Component.hpp"

namespace TUK::Framework
{
	class MaterialAsset;
	class TextureAsset;

	class ImageView final : public Component
	{
	public:
		using Component::Component;

		void OnStart() override;
		void OnPostRender(GPUCommandBuffer& commandBuffer) override;

		[[nodiscard]] const TextureAsset* GetImage() const noexcept;
		void SetImage(const TextureAsset* value) noexcept;

	private:
		const TextureAsset* image = nullptr;
		const MaterialAsset* material = nullptr;
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
