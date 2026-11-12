#pragma once

#include "../Core/Common.hpp"

#include "../Math/Quaternion.hpp"
#include "../Math/Vector3D.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	class MaterialAsset;
	class MeshAsset;

	struct Submodel final
	{
		struct RenderPart final
		{
			const MeshAsset* mesh = nullptr;
			const MaterialAsset* material = nullptr;
		};

		i32 parentIndex = -1;
		std::string name;

		Vector3D<f32> localPosition{ 0.0f, 0.0f, 0.0f };
		Quaternion<f32> localRotation{ 0.0f, 0.0f, 0.0f, 1.0f };
		Vector3D<f32> localScale{ 1.0f, 1.0f, 1.0f };

		std::vector<RenderPart> renderParts;
	};

	class ModelAsset final : public Asset
	{
	public:
		explicit ModelAsset(
			std::filesystem::path path,
			std::vector<Submodel>&& submodels) noexcept;

		[[nodiscard]] std::span<const Submodel> GetSubmodels() const noexcept;

	private:
		std::vector<Submodel> submodels;
	};

	inline ModelAsset::ModelAsset(
		std::filesystem::path path,
		std::vector<Submodel>&& submodels) noexcept
		: Asset(std::move(path))
		, submodels(std::move(submodels))
	{
	}

	inline std::span<const Submodel> ModelAsset::GetSubmodels() const noexcept
	{
		return submodels;
	}
}
