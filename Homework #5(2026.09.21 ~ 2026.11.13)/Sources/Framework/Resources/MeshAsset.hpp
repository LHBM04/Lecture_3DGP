#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <directxtk12/VertexTypes.h>

#include "../Math/ColorRGBA.hpp"
#include "../Math/Vector2D.hpp"
#include "../Math/Vector3D.hpp"

#include "Asset.hpp"

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class MeshAsset final : public Asset
	{
	public:
		MeshAsset() noexcept = default;
		~MeshAsset() noexcept override = default;

		MeshAsset(const MeshAsset&) = delete;
		MeshAsset& operator=(const MeshAsset&) = delete;

		MeshAsset(MeshAsset&&) noexcept = default;
		MeshAsset& operator=(MeshAsset&&) noexcept = default;

		[[nodiscard]] std::string Serialize() const override;
		[[nodiscard]] bool Deserialize(std::string_view text) override;

		[[nodiscard]] std::string_view GetName() const noexcept;
		void SetName(std::string_view value) noexcept;

		[[nodiscard]] const Vector3D<f32>& GetBoundsCenter() const noexcept;
		[[nodiscard]] const Vector3D<f32>& GetBoundsExtents() const noexcept;

		[[nodiscard]] std::span<const Vector3D<f32>> GetPositions() const noexcept;
		[[nodiscard]] std::span<const ColorRGBA<f32>> GetColors() const noexcept;
		[[nodiscard]] std::span<const Vector3D<f32>> GetNormals() const noexcept;
		[[nodiscard]] std::span<const Vector2D<f32>> GetTextureCoords() const noexcept;
		[[nodiscard]] std::span<const DirectX::VertexPositionNormalColor> GetVertices() const noexcept;
		[[nodiscard]] std::span<const u32> GetIndices() const noexcept;

	private:
		std::string name;
		Vector3D<f32> boundsCenter{};
		Vector3D<f32> boundsExtents{};
		std::vector<Vector3D<f32>> positions;
		std::vector<ColorRGBA<f32>> colors;
		std::vector<Vector3D<f32>> normals;
		std::vector<Vector2D<f32>> textureCoords;
		std::vector<DirectX::VertexPositionNormalColor> vertices;
		std::vector<u32> indices;
	};

	inline std::string_view MeshAsset::GetName() const noexcept
	{
		return name;
	}

	inline void MeshAsset::SetName(std::string_view value) noexcept
	{
		name.assign(value);
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

	inline std::span<const DirectX::VertexPositionNormalColor> MeshAsset::GetVertices() const noexcept
	{
		return vertices;
	}

	inline std::span<const u32> MeshAsset::GetIndices() const noexcept
	{
		return indices;
	}
}
