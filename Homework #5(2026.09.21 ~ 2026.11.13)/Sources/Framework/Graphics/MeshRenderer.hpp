#pragma once

#include <cassert>

#include "../Entities/Component.hpp"

namespace TUK::Framework
{
	class MaterialAsset;
	class MeshAsset;

	class MeshRenderer final : public Component
	{
	public:
		using Component::Component;

		void OnRender(GPUCommandBuffer& commandBuffer) override;

		[[nodiscard]] const MeshAsset& GetMesh() const noexcept;
		void SetMesh(const MeshAsset& value) noexcept;

		[[nodiscard]] const MaterialAsset& GetMaterial() const noexcept;
		void SetMaterial(const MaterialAsset& value) noexcept;

	private:
		const MeshAsset* mesh = nullptr;
		const MaterialAsset* material = nullptr;
	};

	inline const MeshAsset& MeshRenderer::GetMesh() const noexcept
	{
		assert(mesh);
		return *mesh;
	}

	inline void MeshRenderer::SetMesh(const MeshAsset& value) noexcept
	{
		mesh = &value;
	}

	inline const MaterialAsset& MeshRenderer::GetMaterial() const noexcept
	{
		assert(material);
		return *material;
	}

	inline void MeshRenderer::SetMaterial(const MaterialAsset& value) noexcept
	{
		material = &value;
	}
}
