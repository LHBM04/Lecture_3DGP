#pragma once

#include "Component.h"

class Material;
class Mesh;

class MeshRenderer final : public Component
{
public:
	MeshRenderer() = default;
	virtual ~MeshRenderer() = default;

	MeshRenderer(const MeshRenderer&) = delete;
	MeshRenderer& operator=(const MeshRenderer&) = delete;

	MeshRenderer(MeshRenderer&&) = delete;
	MeshRenderer& operator=(MeshRenderer&&) = delete;

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
