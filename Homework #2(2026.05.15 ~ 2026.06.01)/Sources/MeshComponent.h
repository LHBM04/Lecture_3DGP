#pragma once
#include "Component.h"

class Mesh;
class Material;

class MeshComponent final : public Component<MeshComponent>
{
public:
	MeshComponent() = default;
	~MeshComponent() = default;

	[[nodiscard]] Mesh* GetMesh() const;
	void SetMesh(Mesh* mesh_);

	[[nodiscard]] Material* GetMaterial() const;
	void SetMaterial(Material* material_);

	void OnRender();

private:
	Mesh* mesh{ nullptr };
	Material* material{ nullptr };
};

inline Mesh* MeshComponent::GetMesh() const
{
	return mesh;
}

inline void MeshComponent::SetMesh(Mesh* mesh_)
{
	mesh = mesh_;
}

inline Material* MeshComponent::GetMaterial() const
{
	return material;
}

inline void MeshComponent::SetMaterial(Material* material_)
{
	material = material_;
}
