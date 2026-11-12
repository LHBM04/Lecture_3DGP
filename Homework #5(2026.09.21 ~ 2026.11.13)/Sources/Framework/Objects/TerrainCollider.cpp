#include "Precompiled.hpp"
#include "TerrainCollider.hpp"

#ifdef _DEBUG
#include "../Graphics/GPUCommandBuffer.hpp"
#endif

#include "../Resources/TerrainAsset.hpp"

#include "GameObject.hpp"
#include "Terrain.hpp"

namespace TUK::Framework
{
	namespace
	{
		template <class TCallback>
		bool VisitTerrainTriangles(
			const TerrainAsset* asset,
			const Matrix4x4<f32>& world,
			const BoundingBox& worldRegion,
			TCallback&& callback) noexcept
		{
			Matrix4x4<f32> inverse{};
			if (!world.TryGetInverse(inverse))
			{
				return false;
			}

			const auto region = worldRegion.GetTransformed(inverse);
			const auto min = region.GetMin();
			const auto max = region.GetMax();

			const f32 halfWidth = asset->GetLocalWidth() * 0.5f;
			const f32 halfDepth = asset->GetLocalDepth() * 0.5f;
			if (max.GetX() < -halfWidth || min.GetX() > halfWidth ||
				max.GetZ() < -halfDepth || min.GetZ() > halfDepth)
			{
				return false;
			}

			const f32 cellSize = asset->GetCellSize();
			const auto cellIndex = [](f32 coordinate, f32 halfSize, f32 spacing, u32 maxCell, bool lower) -> u32
			{
				const f32 grid = (coordinate + halfSize) / spacing;
				const f32 index = lower ? std::floor(grid) - 1.0f : std::floor(grid) + 1.0f;
				return static_cast<u32>(std::clamp(index, 0.0f, static_cast<f32>(maxCell)));
			};

			const u32 minX = cellIndex(min.GetX(), halfWidth, cellSize, asset->GetWidth() - 2, true);
			const u32 maxX = cellIndex(max.GetX(), halfWidth, cellSize, asset->GetWidth() - 2, false);
			const u32 minZ = cellIndex(min.GetZ(), halfDepth, cellSize, asset->GetHeight() - 2, true);
			const u32 maxZ = cellIndex(max.GetZ(), halfDepth, cellSize, asset->GetHeight() - 2, false);

			const auto vertex = [&](u32 x, u32 z) -> Vector3D<f32>
			{
				return world.MultiplyPoint3x4(Vector3D<f32>
				{
					static_cast<f32>(x) * cellSize - halfWidth,
					asset->GetNormalizedHeightAt(x, z) * asset->GetHeightScale(),
					static_cast<f32>(z) * cellSize - halfDepth
				});
			};

			for (u32 z = minZ; z <= maxZ; ++z)
			{
				for (u32 x = minX; x <= maxX; ++x)
				{
					const auto topLeft = vertex(x, z);
					const auto topRight = vertex(x + 1, z);
					const auto bottomLeft = vertex(x, z + 1);
					const auto bottomRight = vertex(x + 1, z + 1);

					if (callback(topLeft, bottomLeft, topRight) ||
						callback(topRight, bottomLeft, bottomRight))
					{
						return true;
					}
				}
			}

			return false;
		}
	}

#ifdef _DEBUG
	void TerrainCollider::OnAwake()
	{
		LoadDebugMaterial("Resources/Materials/DebugTerrainCollider.tukmat");
	}

	void TerrainCollider::OnDrawGizmos(GPUCommandBuffer& commandBuffer)
	{
		if (!IsQueryEnabled() || !PrepareDebugRender(commandBuffer, GetWorldMatrix()))
		{
			return;
		}

		if (auto result = commandBuffer.SetGraphicsConstants(2, GetDebugColor()); !result)
		{
			LOG_ERROR("지형 콜라이더 기즈모 색상을 업로드하지 못했습니다: {}", result.error());
			return;
		}

		const auto& mesh = GetTerrain()->GetGPUMesh();
		commandBuffer.SetVertexBuffer(0, mesh.GetD3DVertexBufferView());
		commandBuffer.SetIndexBuffer(mesh.GetD3DIndexBufferView());
		commandBuffer.DrawIndexedInstanced(mesh.GetIndexCount(), 1);
	}
#endif

	const TerrainAsset* TerrainCollider::GetTerrain() const noexcept
	{
		if (asset)
		{
			return asset;
		}

		const GameObject* owner = GetOwner();
		const Terrain* terrain = owner ? owner->GetComponent<Terrain>() : nullptr;
		return terrain ? terrain->GetTerrain() : nullptr;
	}

	void TerrainCollider::SetTerrain(const TerrainAsset* value) noexcept
	{
		asset = value;
	}

	bool TerrainCollider::IsValid() const noexcept
	{
		const TerrainAsset* terrain = GetTerrain();
		return terrain && terrain->GetWidth() >= 2 && terrain->GetHeight() >= 2 &&
			std::isfinite(terrain->GetCellSize()) && terrain->GetCellSize() > 0.0f &&
			std::isfinite(terrain->GetHeightScale()) &&
			terrain->GetNormalizedHeights().size() == static_cast<usize>(terrain->GetWidth()) * terrain->GetHeight() &&
			HasValidTransform();
	}

	BoundingBox TerrainCollider::GetWorldBounds() const noexcept
	{
		ASSERT_FMT(IsValid(), "지형 콜라이더에는 유효한 하이트필드와 Transform이 필요합니다.");

		const TerrainAsset* terrain = GetTerrain();
		const f32 halfWidth = terrain->GetLocalWidth() * 0.5f;
		const f32 halfDepth = terrain->GetLocalDepth() * 0.5f;
		const f32 heightScale = terrain->GetHeightScale();

		return BoundingBox::CreateFromMinMax(
			Vector3D<f32>{ -halfWidth, std::min(0.0f, heightScale), -halfDepth },
			Vector3D<f32>{ halfWidth, std::max(0.0f, heightScale), halfDepth }).GetTransformed(GetWorldMatrix());
	}

	bool TerrainCollider::Intersects(const BoundingSphere& sphere) const noexcept
	{
		if (!IsQueryEnabled() || !sphere.IsValid())
		{
			return false;
		}

		return VisitTerrainTriangles(GetTerrain(), GetWorldMatrix(), BoundingBox::CreateFromSphere(sphere),
			[&](const auto& v0, const auto& v1, const auto& v2)
			{
				return sphere.Intersects(v0, v1, v2);
			});
	}

	bool TerrainCollider::Intersects(const BoundingOrientedBox& box) const noexcept
	{
		if (!IsQueryEnabled() || !box.IsValid())
		{
			return false;
		}

		const auto corners = box.GetCorners();
		return VisitTerrainTriangles(GetTerrain(), GetWorldMatrix(), BoundingBox::CreateFromPoints(corners),
			[&](const auto& v0, const auto& v1, const auto& v2)
			{
				return box.Intersects(v0, v1, v2);
			});
	}

	bool TerrainCollider::Raycast(
		const Vector3D<f32>& origin,
		const Vector3D<f32>& direction,
		f32& distance,
		f32 maxDistance) const noexcept
	{
		Vector3D<f32> normalizedDirection{};
		if (!IsQueryEnabled() || !TryNormalizeRay(origin, direction, maxDistance, normalizedDirection))
		{
			return false;
		}

		const auto bounds = GetWorldBounds();
		const auto min = bounds.GetMin();
		const auto max = bounds.GetMax();

		f32 enter = 0.0f;
		f32 exit = maxDistance;
		for (usize axis = 0; axis < 3; ++axis)
		{
			if (normalizedDirection[axis] == 0.0f)
			{
				if (origin[axis] < min[axis] || origin[axis] > max[axis])
				{
					return false;
				}
				continue;
			}

			const f32 first = (min[axis] - origin[axis]) / normalizedDirection[axis];
			const f32 second = (max[axis] - origin[axis]) / normalizedDirection[axis];
			enter = std::max(enter, std::min(first, second));
			exit = std::min(exit, std::max(first, second));

			if (enter > exit)
			{
				return false;
			}
		}

		const auto region = BoundingBox::CreateFromPoints(
			origin + normalizedDirection * enter, origin + normalizedDirection * exit);

		bool hit = false;
		f32 closest = maxDistance;
		VisitTerrainTriangles(GetTerrain(), GetWorldMatrix(), region,
			[&](const auto& v0, const auto& v1, const auto& v2)
			{
				f32 candidate = 0.0f;
				if (DirectX::TriangleTests::Intersects(
					Vector3D<f32>::Load(origin), Vector3D<f32>::Load(normalizedDirection),
					Vector3D<f32>::Load(v0), Vector3D<f32>::Load(v1), Vector3D<f32>::Load(v2), candidate) &&
					candidate <= closest)
				{
					closest = candidate;
					hit = true;
				}
				return false;
			});

		if (hit)
		{
			distance = closest;
		}

		return hit;
	}
}
