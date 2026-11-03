#pragma once

#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "Framework/Graphics/GPUMesh.hpp"

#include "Framework/Math/Matrix4x4.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	class MaterialAsset;

	struct ModelPart final
	{
		GPUMesh mesh;
		const MaterialAsset* material;
		Matrix4x4<float> localToModel;
	};

	class ModelAsset : public Asset
	{
	public:
		ModelAsset(std::string name, std::vector<ModelPart> parts) noexcept;
		~ModelAsset() noexcept override = default;

		ModelAsset(const ModelAsset&) = delete;
		ModelAsset& operator=(const ModelAsset&) = delete;
		ModelAsset(ModelAsset&&) noexcept = default;
		ModelAsset& operator=(ModelAsset&&) noexcept = default;

		[[nodiscard]] std::string_view GetName() const noexcept;
		[[nodiscard]] std::span<const ModelPart> GetParts() const noexcept;

	private:
		std::string name;
		std::vector<ModelPart> parts;
	};

	inline ModelAsset::ModelAsset(std::string name, std::vector<ModelPart> parts) noexcept
		: name(std::move(name))
		, parts(std::move(parts))
	{
	}

	inline std::string_view ModelAsset::GetName() const noexcept
	{
		return name;
	}

	inline std::span<const ModelPart> ModelAsset::GetParts() const noexcept
	{
		return parts;
	}
}
