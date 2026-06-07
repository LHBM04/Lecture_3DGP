#pragma once

#include "Component.h"

class Material;
class Terrain;

class TerrainRenderer final : public Component
{
public:
	TerrainRenderer() = default;
	~TerrainRenderer() override = default;

	[[nodiscard]] Terrain* GetTerrain() const noexcept;
	void SetTerrain(Terrain* terrain_) noexcept;

	[[nodiscard]] Material* GetMaterial() const noexcept;
	void SetMaterial(Material* material_) noexcept;

protected:
	void OnRender() override;

private:
	Terrain* terrain{ nullptr };
	Material* material{ nullptr };
};
