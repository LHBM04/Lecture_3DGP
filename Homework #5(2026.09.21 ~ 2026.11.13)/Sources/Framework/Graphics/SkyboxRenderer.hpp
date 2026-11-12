#pragma once

#include "../Objects/Component.hpp"

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

	private:
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
}
