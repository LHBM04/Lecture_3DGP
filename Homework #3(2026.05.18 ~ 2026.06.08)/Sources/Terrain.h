#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "Resource.h"
#include "Vector3D.h"

class Mesh;

class Terrain final : public Resource
{
public:
	Terrain() = default;
	~Terrain() override = default;

	bool Load() override;
	void Unload() override;

	[[nodiscard]] Mesh* GetMesh() const noexcept;
	[[nodiscard]] uint32_t GetWidth() const noexcept;
	[[nodiscard]] uint32_t GetHeight() const noexcept;
	[[nodiscard]] float GetCellSize() const noexcept;
	[[nodiscard]] float GetHeightScale() const noexcept;
	[[nodiscard]] float GetHeightAt(uint32_t x_, uint32_t z_) const noexcept;
	[[nodiscard]] bool ContainsLocalPosition(float localX_, float localZ_) const noexcept;
	[[nodiscard]] float SampleHeightAtLocalPosition(float localX_, float localZ_) const noexcept;

private:
	[[nodiscard]] bool BuildMesh();
	[[nodiscard]] float SampleHeight(uint32_t x_, uint32_t z_) const noexcept;
	[[nodiscard]] Vector3D CalculateNormal(uint32_t x_, uint32_t z_) const noexcept;

private:
	uint32_t width{ 0 };
	uint32_t height{ 0 };
	float cellSize{ 4.0f };
	float heightScale{ 120.0f };
	bool is16Bit{ false };

	std::vector<uint16_t> samples;
	std::unique_ptr<Mesh> mesh;
};
