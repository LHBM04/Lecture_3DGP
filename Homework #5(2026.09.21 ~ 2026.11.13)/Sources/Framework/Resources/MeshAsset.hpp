#pragma once

#include <cassert>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <directxtk12/VertexTypes.h>

#include "../Core/Common.hpp"

#include "../Graphics/GPUMesh.hpp"

#include "../Math/ColorRGBA.hpp"
#include "../Math/Vector2D.hpp"
#include "../Math/Vector3D.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	class MeshAsset final : public Asset
	{
	public:
		using Asset::Asset;

		MeshAsset() noexcept = default;
		~MeshAsset() noexcept override = default;

		MeshAsset(const MeshAsset&) = delete;
		MeshAsset& operator=(const MeshAsset&) = delete;

		MeshAsset(MeshAsset&&) noexcept = default;
		MeshAsset& operator=(MeshAsset&&) noexcept = default;

		[[nodiscard]] std::string Serialize() const override;
		[[nodiscard]] bool Deserialize(std::string_view text) override;

		[[nodiscard]] const Vector3D<f32>& GetBoundsCenter() const noexcept;
		[[nodiscard]] const Vector3D<f32>& GetBoundsExtents() const noexcept;

		[[nodiscard]] std::span<const Vector3D<f32>> GetPositions() const noexcept;
		[[nodiscard]] std::span<const ColorRGBA<f32>> GetColors() const noexcept;
		[[nodiscard]] std::span<const Vector3D<f32>> GetNormals() const noexcept;
		[[nodiscard]] std::span<const Vector2D<f32>> GetTextureCoords() const noexcept;
		[[nodiscard]] std::span<const DirectX::VertexPositionNormalColor> GetVertices() const noexcept;
		[[nodiscard]] std::span<const u32> GetIndices() const noexcept;

		[[nodiscard]] bool HasGPUMesh() const noexcept;
		[[nodiscard]] const GPUMesh& GetGPUMesh() const noexcept;
		void SetGPUMesh(GPUMesh value) noexcept;

	private:
		Vector3D<f32> boundsCenter{};
		Vector3D<f32> boundsExtents{};
		std::vector<Vector3D<f32>> positions;
		std::vector<ColorRGBA<f32>> colors;
		std::vector<Vector3D<f32>> normals;
		std::vector<Vector2D<f32>> textureCoords;
		std::vector<DirectX::VertexPositionNormalColor> vertices;
		std::vector<u32> indices;
		std::optional<GPUMesh> gpuMesh;
	};

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

	inline std::span<const DirectX::VertexPositionNormalColor> MeshAsset::GetVertices() const noexcept
	{
		return vertices;
	}

	inline std::span<const u32> MeshAsset::GetIndices() const noexcept
	{
		return indices;
	}

	inline bool MeshAsset::HasGPUMesh() const noexcept
	{
		return gpuMesh.has_value();
	}

	inline const GPUMesh& MeshAsset::GetGPUMesh() const noexcept
	{
		assert(gpuMesh);
		return *gpuMesh;
	}

	inline void MeshAsset::SetGPUMesh(GPUMesh value) noexcept
	{
		gpuMesh = std::move(value);
	}
}
