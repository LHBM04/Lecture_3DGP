#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "../Core/Common.hpp"

#include "../Math/Quaternion.hpp"
#include "../Math/Vector3D.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	struct Submodel final
	{
		struct RenderPart final
		{
			std::string meshName;
			std::string materialName;
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
		using Asset::Asset;

		ModelAsset() noexcept = default;
		~ModelAsset() noexcept override = default;

		ModelAsset(const ModelAsset&) = delete;
		ModelAsset& operator=(const ModelAsset&) = delete;

		ModelAsset(ModelAsset&&) noexcept = default;
		ModelAsset& operator=(ModelAsset&&) noexcept = default;

		[[nodiscard]] std::string Serialize() const override;
		[[nodiscard]] bool Deserialize(std::string_view text) override;

		[[nodiscard]] std::span<Submodel> GetSubmodels() noexcept;
		[[nodiscard]] std::span<const Submodel> GetSubmodels() const noexcept;

	private:
		std::vector<Submodel> submodels;
	};

	inline std::span<Submodel> ModelAsset::GetSubmodels() noexcept
	{
		return submodels;
	}

	inline std::span<const Submodel> ModelAsset::GetSubmodels() const noexcept
	{
		return submodels;
	}
}
