#pragma once

#include "../Entities/Component.hpp"

namespace TUK::Framework
{
	class MaterialAsset;
	class ModelAsset;

	class MeshRenderer final : public Component
	{
	public:
		explicit MeshRenderer(Entity& owner) noexcept;
		~MeshRenderer() noexcept override = default;

		[[nodiscard]] const ModelAsset* GetModel() const noexcept;
		void SetModel(const ModelAsset* value) noexcept;
		[[nodiscard]] const MaterialAsset* GetMaterial() const noexcept;
		void SetMaterial(const MaterialAsset* value) noexcept;

	protected:
		void OnRender(GPUCommandBuffer& commandBuffer) override;

	private:
		const ModelAsset* model = nullptr;
		const MaterialAsset* material = nullptr;
	};

	inline MeshRenderer::MeshRenderer(Entity& owner) noexcept
		: Component(owner)
	{
	}

	inline const ModelAsset* MeshRenderer::GetModel() const noexcept
	{
		return model;
	}

	inline void MeshRenderer::SetModel(const ModelAsset* value) noexcept
	{
		model = value;
	}

	inline const MaterialAsset* MeshRenderer::GetMaterial() const noexcept
	{
		return material;
	}

	inline void MeshRenderer::SetMaterial(const MaterialAsset* value) noexcept
	{
		material = value;
	}
}
