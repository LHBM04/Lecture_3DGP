#include "Precompiled.hpp"
#include "TerrainAsset.hpp"

namespace TUK::Framework
{
	f32 TerrainAsset::GetNormalizedHeightAt(u32 x, u32 z) const noexcept
	{
		if (width == 0 || height == 0 || normalizedHeights.empty())
		{
			return 0.0f;
		}

		const u32 clampedX = std::min(x, width - 1);
		const u32 clampedZ = std::min(z, height - 1);
		const usize index = static_cast<usize>(clampedZ) * width + clampedX;
		if (index >= normalizedHeights.size())
		{
			return 0.0f;
		}

		return normalizedHeights[index];
	}

	f32 TerrainAsset::SampleNormalizedHeight(f32 gridX, f32 gridZ) const noexcept
	{
		if (width < 2 || height < 2 || normalizedHeights.empty())
		{
			return 0.0f;
		}

		const f32 clampedX = std::clamp(gridX, 0.0f, static_cast<f32>(width - 1));
		const f32 clampedZ = std::clamp(gridZ, 0.0f, static_cast<f32>(height - 1));

		const u32 x0 = static_cast<u32>(std::floor(clampedX));
		const u32 z0 = static_cast<u32>(std::floor(clampedZ));
		const u32 x1 = std::min(x0 + 1, width - 1);
		const u32 z1 = std::min(z0 + 1, height - 1);

		const f32 tx = clampedX - static_cast<f32>(x0);
		const f32 tz = clampedZ - static_cast<f32>(z0);

		const f32 h00 = GetNormalizedHeightAt(x0, z0);
		const f32 h10 = GetNormalizedHeightAt(x1, z0);
		const f32 h01 = GetNormalizedHeightAt(x0, z1);
		const f32 h11 = GetNormalizedHeightAt(x1, z1);

		const f32 h0 = h00 + (h10 - h00) * tx;
		const f32 h1 = h01 + (h11 - h01) * tx;
		return h0 + (h1 - h0) * tz;
	}

	Vector3D<f32> TerrainAsset::CalculateGridNormal(u32 x, u32 z) const noexcept
	{
		return CalculateGridNormal(normalizedHeights, width, height, x, z, cellSize, heightScale);
	}

	Vector3D<f32> TerrainAsset::CalculateGridNormal(
		std::span<const f32> normalizedHeights,
		u32 width,
		u32 height,
		u32 x,
		u32 z,
		f32 cellSize,
		f32 heightScale) noexcept
	{
		if (width < 2 || height < 2 || cellSize <= 0.0f || normalizedHeights.size() < static_cast<usize>(width) * height)
		{
			return Vector3D<f32>::GetUp();
		}

		const auto sampleHeight = [&](u32 sampleX, u32 sampleZ) noexcept -> f32
		{
			const u32 clampedX = std::min(sampleX, width - 1);
			const u32 clampedZ = std::min(sampleZ, height - 1);
			return normalizedHeights[static_cast<usize>(clampedZ) * width + clampedX] * heightScale;
		};

		const u32 left = (x > 0) ? (x - 1) : x;
		const u32 right = (x + 1 < width) ? (x + 1) : x;
		const u32 down = (z > 0) ? (z - 1) : z;
		const u32 up = (z + 1 < height) ? (z + 1) : z;

		const f32 heightL = sampleHeight(left, z);
		const f32 heightR = sampleHeight(right, z);
		const f32 heightD = sampleHeight(x, down);
		const f32 heightU = sampleHeight(x, up);

		const f32 spanX = static_cast<f32>(right - left) * cellSize;
		const f32 spanZ = static_cast<f32>(up - down) * cellSize;

		const Vector3D<f32> tangentX{ spanX, heightR - heightL, 0.0f };
		const Vector3D<f32> tangentZ{ 0.0f, heightU - heightD, spanZ };
		Vector3D<f32> normal = Vector3D<f32>::Cross(tangentZ, tangentX);
		normal.Normalize();
		return normal;
	}
}
