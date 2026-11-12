#pragma once

#include "../Math/ColorRGBA.hpp"

#include "../GameObjects/Component.hpp"

namespace TUK::Framework
{
	class MaterialAsset;
	class MeshAsset;

	class MeshRenderer final : public Component
	{
	public:
		using Component::Component;

		void OnRender(GPUCommandBuffer& commandBuffer) override;

		[[nodiscard]] const MeshAsset* GetMesh() const noexcept;
		void SetMesh(const MeshAsset* value) noexcept;

		[[nodiscard]] const MaterialAsset* GetMaterial() const noexcept;
		void SetMaterial(const MaterialAsset* value) noexcept;

		[[nodiscard]] const ColorRGBA<f32>& GetColor() const noexcept;
		void SetColor(const ColorRGBA<f32>& value) noexcept;

	private:
		ColorRGBA<f32> color = ColorRGBA<f32>::GetWhite();
		const MeshAsset* mesh = nullptr;
		const MaterialAsset* material = nullptr;
	};

	inline const MeshAsset* MeshRenderer::GetMesh() const noexcept
	{
		return mesh;
	}

	inline void MeshRenderer::SetMesh(const MeshAsset* value) noexcept
	{
		mesh = value;
	}

	inline const MaterialAsset* MeshRenderer::GetMaterial() const noexcept
	{
		return material;
	}

	inline void MeshRenderer::SetMaterial(const MaterialAsset* value) noexcept
	{
		material = value;
	}

	inline const ColorRGBA<f32>& MeshRenderer::GetColor() const noexcept
	{
		return color;
	}

	inline void MeshRenderer::SetColor(const ColorRGBA<f32>& value) noexcept
	{
		color = value;
	}
}
