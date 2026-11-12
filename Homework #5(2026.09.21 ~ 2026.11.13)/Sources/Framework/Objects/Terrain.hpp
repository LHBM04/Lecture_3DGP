#pragma once

#include "../Core/Common.hpp"

#include "../Math/Vector2D.hpp"
#include "../Math/Vector3D.hpp"

#include "Component.hpp"

namespace TUK::Framework
{
	template <Scalarable TScalar>
	class Matrix4x4;

	class TerrainAsset;

	class Terrain final : public Component
	{
	public:
		using Component::Component;

		[[nodiscard]] const TerrainAsset* GetTerrain() const noexcept;
		void SetTerrain(const TerrainAsset* value) noexcept;

		[[nodiscard]] f32 GetCellSize() const noexcept;
		[[nodiscard]] f32 GetHeightScale() const noexcept;
		[[nodiscard]] const Vector2D<f32>& GetUVTiling() const noexcept;

		[[nodiscard]] bool IsValid() const noexcept;

		[[nodiscard]] f32 GetLocalWidth() const noexcept;
		[[nodiscard]] f32 GetLocalDepth() const noexcept;

		[[nodiscard]] bool ContainsLocalPosition(f32 localX, f32 localZ) const noexcept;
		[[nodiscard]] f32 SampleHeightAtLocalPosition(f32 localX, f32 localZ) const noexcept;
		[[nodiscard]] Vector3D<f32> SampleNormalAtLocalPosition(f32 localX, f32 localZ) const noexcept;

		[[nodiscard]] bool ContainsWorldPosition(const Vector3D<f32>& worldPosition) const noexcept;
		[[nodiscard]] f32 SampleHeightAtWorldPosition(const Vector3D<f32>& worldPosition) const noexcept;
		[[nodiscard]] Vector3D<f32> SampleNormalAtWorldPosition(const Vector3D<f32>& worldPosition) const noexcept;

	private:
		[[nodiscard]] bool TryGetWorldMatrices(Matrix4x4<f32>& outWorld, Matrix4x4<f32>& outInverseWorld) const noexcept;

		const TerrainAsset* asset = nullptr;
	};

	inline const TerrainAsset* Terrain::GetTerrain() const noexcept
	{
		return asset;
	}

	inline void Terrain::SetTerrain(const TerrainAsset* value) noexcept
	{
		asset = value;
	}
}
