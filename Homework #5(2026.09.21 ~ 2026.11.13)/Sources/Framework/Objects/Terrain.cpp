#include "Precompiled.hpp"
#include "Terrain.hpp"

#include "../Math/Matrix4x4.hpp"

#include "../Resources/TerrainAsset.hpp"

#include "GameObject.hpp"
#include "Transform.hpp"

namespace TUK::Framework
{
	f32 Terrain::GetCellSize() const noexcept
	{
		return asset ? asset->GetCellSize() : 0.0f;
	}

	f32 Terrain::GetHeightScale() const noexcept
	{
		return asset ? asset->GetHeightScale() : 0.0f;
	}

	const Vector2D<f32>& Terrain::GetUVTiling() const noexcept
	{
		static const Vector2D<f32> DEFAULT_UV_TILING{ 1.0f, 1.0f };
		return asset ? asset->GetUVTiling() : DEFAULT_UV_TILING;
	}

	bool Terrain::IsValid() const noexcept
	{
		return asset != nullptr && asset->GetWidth() >= 2 && asset->GetHeight() >= 2 && asset->GetCellSize() > 0.0f;
	}

	f32 Terrain::GetLocalWidth() const noexcept
	{
		return IsValid() ? asset->GetLocalWidth() : 0.0f;
	}

	f32 Terrain::GetLocalDepth() const noexcept
	{
		return IsValid() ? asset->GetLocalDepth() : 0.0f;
	}

	bool Terrain::ContainsLocalPosition(f32 localX, f32 localZ) const noexcept
	{
		if (!IsValid())
		{
			return false;
		}

		const f32 halfWidth = GetLocalWidth() * 0.5f;
		const f32 halfDepth = GetLocalDepth() * 0.5f;

		return localX >= -halfWidth && localX <= halfWidth
			&& localZ >= -halfDepth && localZ <= halfDepth;
	}

	f32 Terrain::SampleHeightAtLocalPosition(f32 localX, f32 localZ) const noexcept
	{
		if (!IsValid())
		{
			return 0.0f;
		}

		const f32 cellSize = asset->GetCellSize();
		const f32 halfWidth = GetLocalWidth() * 0.5f;
		const f32 halfDepth = GetLocalDepth() * 0.5f;
		const f32 gridX = (localX + halfWidth) / cellSize;
		const f32 gridZ = (localZ + halfDepth) / cellSize;

		return asset->SampleNormalizedHeight(gridX, gridZ) * asset->GetHeightScale();
	}

	Vector3D<f32> Terrain::SampleNormalAtLocalPosition(f32 localX, f32 localZ) const noexcept
	{
		if (!IsValid())
		{
			return Vector3D<f32>::GetUp();
		}

		const u32 width = asset->GetWidth();
		const u32 height = asset->GetHeight();
		const f32 cellSize = asset->GetCellSize();
		const f32 halfWidth = GetLocalWidth() * 0.5f;
		const f32 halfDepth = GetLocalDepth() * 0.5f;

		const f32 gridX = std::clamp((localX + halfWidth) / cellSize, 0.0f, static_cast<f32>(width - 1));
		const f32 gridZ = std::clamp((localZ + halfDepth) / cellSize, 0.0f, static_cast<f32>(height - 1));

		const u32 x0 = static_cast<u32>(std::floor(gridX));
		const u32 z0 = static_cast<u32>(std::floor(gridZ));
		const u32 x1 = std::min(x0 + 1, width - 1);
		const u32 z1 = std::min(z0 + 1, height - 1);

		const f32 tx = gridX - static_cast<f32>(x0);
		const f32 tz = gridZ - static_cast<f32>(z0);

		const Vector3D<f32> n00 = asset->CalculateGridNormal(x0, z0);
		const Vector3D<f32> n10 = asset->CalculateGridNormal(x1, z0);
		const Vector3D<f32> n01 = asset->CalculateGridNormal(x0, z1);
		const Vector3D<f32> n11 = asset->CalculateGridNormal(x1, z1);

		const Vector3D<f32> n0 = Vector3D<f32>::Lerp(n00, n10, tx);
		const Vector3D<f32> n1 = Vector3D<f32>::Lerp(n01, n11, tx);
		Vector3D<f32> normal = Vector3D<f32>::Lerp(n0, n1, tz);
		normal.Normalize();
		return normal;
	}

	bool Terrain::ContainsWorldPosition(const Vector3D<f32>& worldPosition) const noexcept
	{
		if (!IsValid())
		{
			return false;
		}

		Matrix4x4<f32> worldMatrix{};
		Matrix4x4<f32> inverseWorld{};
		if (!TryGetWorldMatrices(worldMatrix, inverseWorld))
		{
			return ContainsLocalPosition(worldPosition.GetX(), worldPosition.GetZ());
		}

		const Vector3D<f32> localPosition = inverseWorld.MultiplyPoint3x4(worldPosition);
		return ContainsLocalPosition(localPosition.GetX(), localPosition.GetZ());
	}

	f32 Terrain::SampleHeightAtWorldPosition(const Vector3D<f32>& worldPosition) const noexcept
	{
		if (!IsValid())
		{
			return 0.0f;
		}

		Matrix4x4<f32> worldMatrix{};
		Matrix4x4<f32> inverseWorld{};
		if (!TryGetWorldMatrices(worldMatrix, inverseWorld))
		{
			return SampleHeightAtLocalPosition(worldPosition.GetX(), worldPosition.GetZ());
		}

		const Vector3D<f32> localPosition = inverseWorld.MultiplyPoint3x4(worldPosition);
		const f32 localHeight = SampleHeightAtLocalPosition(localPosition.GetX(), localPosition.GetZ());
		const Vector3D<f32> worldSurface = worldMatrix.MultiplyPoint3x4(
			Vector3D<f32>(localPosition.GetX(), localHeight, localPosition.GetZ()));
		return worldSurface.GetY();
	}

	Vector3D<f32> Terrain::SampleNormalAtWorldPosition(const Vector3D<f32>& worldPosition) const noexcept
	{
		if (!IsValid())
		{
			return Vector3D<f32>::GetUp();
		}

		Matrix4x4<f32> worldMatrix{};
		Matrix4x4<f32> inverseWorld{};
		if (!TryGetWorldMatrices(worldMatrix, inverseWorld))
		{
			return SampleNormalAtLocalPosition(worldPosition.GetX(), worldPosition.GetZ());
		}

		const Vector3D<f32> localPosition = inverseWorld.MultiplyPoint3x4(worldPosition);
		const Vector3D<f32> localNormal = SampleNormalAtLocalPosition(localPosition.GetX(), localPosition.GetZ());
		Vector3D<f32> worldNormal = inverseWorld.GetTranspose().MultiplyVector(localNormal);
		worldNormal.Normalize();
		return worldNormal;
	}

	bool Terrain::TryGetWorldMatrices(Matrix4x4<f32>& outWorld, Matrix4x4<f32>& outInverseWorld) const noexcept
	{
		const GameObject* owner = GetOwner();
		const Transform* transform = owner ? owner->GetComponent<Transform>() : nullptr;
		if (!transform)
		{
			return false;
		}

		outWorld = transform->GetWorldMatrix();
		return outWorld.TryGetInverse(outInverseWorld);
	}
}
