#pragma once
#include "Component.h"

class Mesh;
class Material;

class MeshRenderer final : public Component
{
public:
	MeshRenderer() = default;
	~MeshRenderer() override = default;

	[[nodiscard]] Mesh* GetMesh() const noexcept;
	void SetMesh(Mesh* mesh_) noexcept;

	[[nodiscard]] Material* GetMaterial() const noexcept;
	void SetMaterial(Material* material_) noexcept;

	void OnRender() override;

private:
	Mesh* mesh{ nullptr };
	Material* material{ nullptr };
};

inline Mesh* MeshRenderer::GetMesh() const noexcept
{
	return mesh;
}

inline void MeshRenderer::SetMesh(Mesh* mesh_) noexcept
{
	mesh = mesh_;
}

inline Material* MeshRenderer::GetMaterial() const noexcept
{
	return material;
}

inline void MeshRenderer::SetMaterial(Material* material_) noexcept
{
	material = material_;
}
