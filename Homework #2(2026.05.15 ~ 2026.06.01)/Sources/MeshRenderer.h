#pragma once

#include "Component.h"
#include "Renderable.h"

class Material;
class Mesh;

class MeshRenderer : public Component, public Renderable
{
public:
	virtual ~MeshRenderer() = default;

	[[nodiscard]] Mesh* GetMesh() noexcept;
	[[nodiscard]] const Mesh* GetMesh() const noexcept;
	void SetMesh(Mesh* mesh_) noexcept;

	[[nodiscard]] Material* GetMaterial() noexcept;
	[[nodiscard]] const Material* GetMaterial() const noexcept;
	void SetMaterial(Material* material_) noexcept;

protected:
	virtual void OnRender() override;

private:
	Mesh* mesh{ nullptr };
	Material* material{ nullptr };
};
