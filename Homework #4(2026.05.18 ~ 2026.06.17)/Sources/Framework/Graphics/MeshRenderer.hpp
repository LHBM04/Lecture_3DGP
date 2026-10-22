#pragma once

#include "../Entities/Component.hpp"

namespace TUK::Framework
{
	class Material;
	class Mesh;

	class MeshRenderer : public Component
	{
	public:
		MeshRenderer() noexcept;
		~MeshRenderer() noexcept override;

		[[nodiscard]] Mesh& GetMesh() const noexcept;
		void SetMesh(Mesh& value) noexcept;

		[[nodiscard]] Material& GetMaterial() const noexcept;
		void SetMaterial(Material& value) noexcept;

	protected:
		void OnRender(CommandBuffer& commandBuffer) override;

	private:
		Mesh* mesh;
		Material* material;
	};
}
