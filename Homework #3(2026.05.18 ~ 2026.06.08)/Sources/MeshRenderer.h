#pragma once

#include "Component.h"

class Material;
class Mesh;

class MeshRenderer final : public Component
{
public:
	MeshRenderer() noexcept = default;
	~MeshRenderer() noexcept override = default;

	void SetMesh(Mesh* mesh_) noexcept;
	[[nodiscard]] Mesh* GetMesh() noexcept;
	[[nodiscard]] const Mesh* GetMesh() const noexcept;

	void SetMaterial(Material* material_) noexcept;
	[[nodiscard]] Material* GetMaterial() noexcept;
	[[nodiscard]] const Material* GetMaterial() const noexcept;

	void OnRender(ID3D12GraphicsCommandList* commandList_) override;

private:
	Mesh* mesh{ nullptr };
	Material* material{ nullptr };
};
