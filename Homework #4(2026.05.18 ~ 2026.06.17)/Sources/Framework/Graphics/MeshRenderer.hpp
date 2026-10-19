#pragma once

#include "Framework/Entities/Component.hpp"

namespace TUK::Framework
{
	class Material;
	class Mesh;

	class MeshRenderer : public Component
	{
	public:
		MeshRenderer() noexcept;
		~MeshRenderer() noexcept override;

		[[nodiscard]] Mesh* GetMesh() const noexcept;
		void SetMesh(Mesh* mesh) noexcept;

		[[nodiscard]] Material* GetMaterial() const noexcept;
		void SetMaterial(Material* material) noexcept;

	protected:
		void OnRender(CommandBuffer& renderer) override;

	private:
		Mesh* mesh{ nullptr };
		Material* material{ nullptr };
	};
}
