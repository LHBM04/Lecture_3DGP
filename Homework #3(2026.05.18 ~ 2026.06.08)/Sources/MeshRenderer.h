#pragma once

#include "Component.h"

class Material;
class Mesh;

class MeshRenderer final : public Component
{
public:
	MeshRenderer() = default;
	~MeshRenderer() override = default;

	[[nodiscard]] Mesh* GetMesh() const noexcept;
	void SetMesh(Mesh* mesh_) noexcept;

	[[nodiscard]] Material* GetMaterial() const noexcept;
	void SetMaterial(Material* material_) noexcept;

protected:
	void OnRender() override;

private:
	Mesh* mesh{ nullptr };
	Material* material{ nullptr };
};
