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

	class MeshAsset final : public Asset
	{
	public:
		explicit MeshAsset(std::filesystem::path path, GPUMesh&& gpuMesh) noexcept;

		[[nodiscard]] const Vector3D<f32>& GetBoundsCenter() const noexcept;
		void SetBoundsCenter(const Vector3D<f32>& value) noexcept;

		[[nodiscard]] const Vector3D<f32>& GetBoundsExtents() const noexcept;
		void SetBoundsExtents(const Vector3D<f32>& value) noexcept;

		[[nodiscard]] std::span<const Vector3D<f32>> GetPositions() const noexcept;
		void SetPositions(std::vector<Vector3D<f32>>&& value) noexcept;

		[[nodiscard]] std::span<const ColorRGBA<f32>> GetColors() const noexcept;
		void SetColors(std::vector<ColorRGBA<f32>>&& value) noexcept;

		[[nodiscard]] std::span<const Vector3D<f32>> GetNormals() const noexcept;
		void SetNormals(std::vector<Vector3D<f32>>&& value) noexcept;

		[[nodiscard]] std::span<const Vector2D<f32>> GetTextureCoords() const noexcept;
		void SetTextureCoords(std::vector<Vector2D<f32>>&& value) noexcept;

		[[nodiscard]] std::span<const MeshVertex> GetVertices() const noexcept;
		void SetVertices(std::vector<MeshVertex>&& value) noexcept;

		[[nodiscard]] std::span<const u32> GetIndices() const noexcept;
		void SetIndices(std::vector<u32>&& value) noexcept;

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

	inline MeshAsset::MeshAsset(std::filesystem::path path, GPUMesh&& gpuMesh) noexcept
		: Asset(std::move(path))
		, gpuMesh(std::move(gpuMesh))
	{
	}

	inline const Vector3D<f32>& MeshAsset::GetBoundsCenter() const noexcept
	{
		return boundsCenter;
	}

	inline void MeshAsset::SetBoundsCenter(const Vector3D<f32>& value) noexcept
	{
		boundsCenter = value;
	}

	inline const Vector3D<f32>& MeshAsset::GetBoundsExtents() const noexcept
	{
		return boundsExtents;
	}

	inline void MeshAsset::SetBoundsExtents(const Vector3D<f32>& value) noexcept
	{
		boundsExtents = value;
	}

	inline std::span<const Vector3D<f32>> MeshAsset::GetPositions() const noexcept
	{
		return positions;
	}

	inline void MeshAsset::SetPositions(std::vector<Vector3D<f32>>&& value) noexcept
	{
		positions = std::move(value);
	}

	inline std::span<const ColorRGBA<f32>> MeshAsset::GetColors() const noexcept
	{
		return colors;
	}

	inline void MeshAsset::SetColors(std::vector<ColorRGBA<f32>>&& value) noexcept
	{
		colors = std::move(value);
	}

	inline std::span<const Vector3D<f32>> MeshAsset::GetNormals() const noexcept
	{
		return normals;
	}

	inline void MeshAsset::SetNormals(std::vector<Vector3D<f32>>&& value) noexcept
	{
		normals = std::move(value);
	}

	inline std::span<const Vector2D<f32>> MeshAsset::GetTextureCoords() const noexcept
	{
		return textureCoords;
	}

	inline void MeshAsset::SetTextureCoords(std::vector<Vector2D<f32>>&& value) noexcept
	{
		textureCoords = std::move(value);
	}

	inline std::span<const MeshVertex> MeshAsset::GetVertices() const noexcept
	{
		return vertices;
	}

	inline void MeshAsset::SetVertices(std::vector<MeshVertex>&& value) noexcept
	{
		vertices = std::move(value);
	}

	inline std::span<const u32> MeshAsset::GetIndices() const noexcept
	{
		return indices;
	}

	inline void MeshAsset::SetIndices(std::vector<u32>&& value) noexcept
	{
		indices = std::move(value);
	}

	inline const GPUMesh& MeshAsset::GetGPUMesh() const noexcept
	{
		return gpuMesh;
	}
}
