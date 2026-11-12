#pragma once

#include "../Core/Common.hpp"

#include "../Graphics/GPUMesh.hpp"

#include "../Math/Vector2D.hpp"
#include "../Math/Vector3D.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	class TerrainAsset final : public Asset
	{
	public:
		explicit TerrainAsset(
			std::filesystem::path path,
			u32 width,
			u32 height,
			bool is16Bit,
			f32 cellSize,
			f32 heightScale,
			const Vector2D<f32>& uvTiling,
			std::vector<f32>&& normalizedHeights,
			GPUMesh&& gpuMesh) noexcept;

		[[nodiscard]] u32 GetWidth() const noexcept;
		[[nodiscard]] u32 GetHeight() const noexcept;
		[[nodiscard]] bool Is16Bit() const noexcept;

		[[nodiscard]] f32 GetCellSize() const noexcept;
		[[nodiscard]] f32 GetHeightScale() const noexcept;
		[[nodiscard]] const Vector2D<f32>& GetUVTiling() const noexcept;

		[[nodiscard]] f32 GetLocalWidth() const noexcept;
		[[nodiscard]] f32 GetLocalDepth() const noexcept;

		[[nodiscard]] std::span<const f32> GetNormalizedHeights() const noexcept;
		[[nodiscard]] f32 GetNormalizedHeightAt(u32 x, u32 z) const noexcept;
		[[nodiscard]] f32 SampleNormalizedHeight(f32 gridX, f32 gridZ) const noexcept;
		[[nodiscard]] Vector3D<f32> CalculateGridNormal(u32 x, u32 z) const noexcept;
		[[nodiscard]] static Vector3D<f32> CalculateGridNormal(
			std::span<const f32> normalizedHeights,
			u32 width,
			u32 height,
			u32 x,
			u32 z,
			f32 cellSize,
			f32 heightScale) noexcept;

		[[nodiscard]] const GPUMesh& GetGPUMesh() const noexcept;

	private:
		u32 width = 0;
		u32 height = 0;
		bool is16Bit = false;
		f32 cellSize = 4.0f;
		f32 heightScale = 120.0f;
		Vector2D<f32> uvTiling{ 1.0f, 1.0f };
		std::vector<f32> normalizedHeights;
		GPUMesh gpuMesh;
	};

	inline TerrainAsset::TerrainAsset(
		std::filesystem::path path,
		u32 width,
		u32 height,
		bool is16Bit,
		f32 cellSize,
		f32 heightScale,
		const Vector2D<f32>& uvTiling,
		std::vector<f32>&& normalizedHeights,
		GPUMesh&& gpuMesh) noexcept
		: Asset(std::move(path))
		, width(width)
		, height(height)
		, is16Bit(is16Bit)
		, cellSize(cellSize)
		, heightScale(heightScale)
		, uvTiling(uvTiling)
		, normalizedHeights(std::move(normalizedHeights))
		, gpuMesh(std::move(gpuMesh))
	{
	}

	inline u32 TerrainAsset::GetWidth() const noexcept
	{
		return width;
	}

	inline u32 TerrainAsset::GetHeight() const noexcept
	{
		return height;
	}

	inline bool TerrainAsset::Is16Bit() const noexcept
	{
		return is16Bit;
	}

	inline f32 TerrainAsset::GetCellSize() const noexcept
	{
		return cellSize;
	}

	inline f32 TerrainAsset::GetHeightScale() const noexcept
	{
		return heightScale;
	}

	inline const Vector2D<f32>& TerrainAsset::GetUVTiling() const noexcept
	{
		return uvTiling;
	}

	inline f32 TerrainAsset::GetLocalWidth() const noexcept
	{
		if (width < 2 || cellSize <= 0.0f)
		{
			return 0.0f;
		}

		return static_cast<f32>(width - 1) * cellSize;
	}

	inline f32 TerrainAsset::GetLocalDepth() const noexcept
	{
		if (height < 2 || cellSize <= 0.0f)
		{
			return 0.0f;
		}

		return static_cast<f32>(height - 1) * cellSize;
	}

	inline std::span<const f32> TerrainAsset::GetNormalizedHeights() const noexcept
	{
		return normalizedHeights;
	}

	inline const GPUMesh& TerrainAsset::GetGPUMesh() const noexcept
	{
		return gpuMesh;
	}
}
