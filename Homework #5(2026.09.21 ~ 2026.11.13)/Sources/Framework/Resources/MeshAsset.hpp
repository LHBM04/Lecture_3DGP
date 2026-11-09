#pragma once

#include <filesystem>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include <directxmath/DirectXMath.h>

#include "../Core/Common.hpp"

#include "../Graphics/GPUMesh.hpp"

#include "../Math/ColorRGBA.hpp"
#include "../Math/Vector2D.hpp"
#include "../Math/Vector3D.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	struct MeshVertex final
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 uv0;
	};

	struct MeshAssetData final
	{
		Vector3D<f32> boundsCenter{};
		Vector3D<f32> boundsExtents{};
		std::vector<Vector3D<f32>> positions;
		std::vector<ColorRGBA<f32>> colors;
		std::vector<Vector3D<f32>> normals;
		std::vector<Vector2D<f32>> textureCoords;
		std::vector<MeshVertex> vertices;
		std::vector<u32> indices;
		GPUMesh gpuMesh;
	};

	class MeshAsset final : public Asset
	{
	public:
		explicit MeshAsset(
			std::filesystem::path path,
			MeshAssetData&& data) noexcept;

		[[nodiscard]] const Vector3D<f32>& GetBoundsCenter() const noexcept;
		[[nodiscard]] const Vector3D<f32>& GetBoundsExtents() const noexcept;

		[[nodiscard]] std::span<const Vector3D<f32>> GetPositions() const noexcept;
		[[nodiscard]] std::span<const ColorRGBA<f32>> GetColors() const noexcept;
		[[nodiscard]] std::span<const Vector3D<f32>> GetNormals() const noexcept;
		[[nodiscard]] std::span<const Vector2D<f32>> GetTextureCoords() const noexcept;
		[[nodiscard]] std::span<const MeshVertex> GetVertices() const noexcept;
		[[nodiscard]] std::span<const u32> GetIndices() const noexcept;

		[[nodiscard]] const GPUMesh& GetGPUMesh() const noexcept;

	private:
		Vector3D<f32> boundsCenter{};
		Vector3D<f32> boundsExtents{};
		std::vector<Vector3D<f32>> positions;
		std::vector<ColorRGBA<f32>> colors;
		std::vector<Vector3D<f32>> normals;
		std::vector<Vector2D<f32>> textureCoords;
		std::vector<MeshVertex> vertices;
		std::vector<u32> indices;
		GPUMesh gpuMesh;
	};

	inline MeshAsset::MeshAsset(
		std::filesystem::path path,
		MeshAssetData&& data) noexcept
		: Asset(std::move(path))
		, boundsCenter(data.boundsCenter)
		, boundsExtents(data.boundsExtents)
		, positions(std::move(data.positions))
		, colors(std::move(data.colors))
		, normals(std::move(data.normals))
		, textureCoords(std::move(data.textureCoords))
		, vertices(std::move(data.vertices))
		, indices(std::move(data.indices))
		, gpuMesh(std::move(data.gpuMesh))
	{
	}

	inline const Vector3D<f32>& MeshAsset::GetBoundsCenter() const noexcept
	{
		return boundsCenter;
	}

	inline const Vector3D<f32>& MeshAsset::GetBoundsExtents() const noexcept
	{
		return boundsExtents;
	}

	inline std::span<const Vector3D<f32>> MeshAsset::GetPositions() const noexcept
	{
		return positions;
	}

	inline std::span<const ColorRGBA<f32>> MeshAsset::GetColors() const noexcept
	{
		return colors;
	}

	inline std::span<const Vector3D<f32>> MeshAsset::GetNormals() const noexcept
	{
		return normals;
	}

	inline std::span<const Vector2D<f32>> MeshAsset::GetTextureCoords() const noexcept
	{
		return textureCoords;
	}

	inline std::span<const MeshVertex> MeshAsset::GetVertices() const noexcept
	{
		return vertices;
	}

	inline std::span<const u32> MeshAsset::GetIndices() const noexcept
	{
		return indices;
	}

	inline const GPUMesh& MeshAsset::GetGPUMesh() const noexcept
	{
		return gpuMesh;
	}
}
