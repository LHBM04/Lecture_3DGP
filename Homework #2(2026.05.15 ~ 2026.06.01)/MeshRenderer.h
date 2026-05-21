#pragma once

#include "Component.h"

class Mesh;
class Material;
class RenderTarget;

class MeshRenderer final : public Component
{
public:
	MeshRenderer() noexcept = default;
	virtual ~MeshRenderer() noexcept override = default;

	void SetMesh(Mesh* mesh_) noexcept;
	[[nodiscard]] Mesh* GetMesh() noexcept;
	[[nodiscard]] const Mesh* GetMesh() const noexcept;

	void SetMaterial(Material* material_) noexcept;
	[[nodiscard]] Material* GetMaterial() noexcept;
	[[nodiscard]] const Material* GetMaterial() const noexcept;

protected:
	virtual void OnRender(RenderTarget& renderTarget_) override;

private:
	Mesh* mesh{ nullptr };
	Material* material{ nullptr };
};
