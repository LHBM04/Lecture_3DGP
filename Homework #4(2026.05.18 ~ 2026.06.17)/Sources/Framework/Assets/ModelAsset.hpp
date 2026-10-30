#pragma once

#include <type_traits>
#include <utility>

#include "Asset.hpp"
#include "Framework/Graphics/D3DMesh.hpp"

#include "Framework/Math/ColorRGB.hpp"
#include "Framework/Math/Vector3D.hpp"

namespace TUK::Framework
{
	struct ModelVertex final
	{
		Vector3D<float> position{};
		Vector3D<float> normal{};
		ColorRGB<float> color{ 1.0f, 1.0f, 1.0f };
	};

	static_assert(std::is_standard_layout_v<Vector3D<float>>);
	static_assert(sizeof(Vector3D<float>) == sizeof(float) * 3);
	static_assert(std::is_standard_layout_v<ColorRGB<float>>);
	static_assert(sizeof(ColorRGB<float>) == sizeof(float) * 3);
	static_assert(std::is_standard_layout_v<ModelVertex>);

	class ModelAsset : public Asset
	{
	public:
		explicit ModelAsset(D3DMesh mesh) noexcept;
		~ModelAsset() noexcept override = default;

		ModelAsset(const ModelAsset&) = delete;
		ModelAsset& operator=(const ModelAsset&) = delete;

		ModelAsset(ModelAsset&&) noexcept = default;
		ModelAsset& operator=(ModelAsset&&) noexcept = default;

		[[nodiscard]] const D3DMesh& GetMesh() const noexcept;

	private:
		D3DMesh mesh;
	};

	inline ModelAsset::ModelAsset(D3DMesh mesh) noexcept
		: mesh(std::move(mesh))
	{
	}

	inline const D3DMesh& ModelAsset::GetMesh() const noexcept
	{
		return mesh;
	}
}
