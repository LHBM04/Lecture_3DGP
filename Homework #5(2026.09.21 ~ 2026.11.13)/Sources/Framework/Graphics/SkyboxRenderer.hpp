#pragma once

#include "../Math/ColorRGBA.hpp"

#include "../GameObjects/Component.hpp"

namespace TUK::Framework
{
	class CubemapAsset;
	class MaterialAsset;

	class SkyboxRenderer final : public Component
	{
	public:
		using Component::Component;

		void OnRender(GPUCommandBuffer& commandBuffer) override;

		[[nodiscard]] const CubemapAsset* GetCubemap() const noexcept;
		void SetCubemap(const CubemapAsset* value) noexcept;

		[[nodiscard]] const MaterialAsset* GetMaterial() const noexcept;
		void SetMaterial(const MaterialAsset* value) noexcept;

		[[nodiscard]] const ColorRGBA<f32>& GetColor() const noexcept;
		void SetColor(const ColorRGBA<f32>& value) noexcept;

	private:
		ColorRGBA<f32> color = ColorRGBA<f32>::GetWhite();
		const CubemapAsset* cubemap = nullptr;
		const MaterialAsset* material = nullptr;
	};

	inline const CubemapAsset* SkyboxRenderer::GetCubemap() const noexcept
	{
		return cubemap;
	}

	inline void SkyboxRenderer::SetCubemap(const CubemapAsset* value) noexcept
	{
		cubemap = value;
	}

	inline const MaterialAsset* SkyboxRenderer::GetMaterial() const noexcept
	{
		return material;
	}

	inline void SkyboxRenderer::SetMaterial(const MaterialAsset* value) noexcept
	{
		material = value;
	}

	inline const ColorRGBA<f32>& SkyboxRenderer::GetColor() const noexcept
	{
		return color;
	}

	inline void SkyboxRenderer::SetColor(const ColorRGBA<f32>& value) noexcept
	{
		color = value;
	}
}
