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
	struct ModelRendererReference final
	{
		std::string meshName;
		std::string materialName;
	};

	struct ModelNode final
	{
		i32 index = 0;
		i32 parentIndex = -1;
		std::string name;
		Vector3D<f32> localPosition{ 0.0f, 0.0f, 0.0f };
		Quaternion<f32> localRotation{ 0.0f, 0.0f, 0.0f, 1.0f };
		Vector3D<f32> localScale{ 1.0f, 1.0f, 1.0f };
		std::vector<ModelRendererReference> renderers;
	};

	class ModelAsset final : public Asset
	{
	public:
		ModelAsset() noexcept = default;
		~ModelAsset() noexcept override = default;

		ModelAsset(const ModelAsset&) = delete;
		ModelAsset& operator=(const ModelAsset&) = delete;
		ModelAsset(ModelAsset&&) noexcept = default;
		ModelAsset& operator=(ModelAsset&&) noexcept = default;

		[[nodiscard]] std::string Serialize() const override;
		[[nodiscard]] bool Deserialize(std::string_view text) override;

		[[nodiscard]] std::string_view GetName() const noexcept;
		void SetName(std::string_view value) noexcept;

		[[nodiscard]] std::span<ModelNode> GetNodes() noexcept;
		[[nodiscard]] std::span<const ModelNode> GetNodes() const noexcept;

	private:
		std::string name;
		std::vector<ModelNode> nodes;
	};

	inline std::string_view ModelAsset::GetName() const noexcept
	{
		return name;
	}

	inline void ModelAsset::SetName(std::string_view value) noexcept
	{
		name.assign(value);
	}

	inline std::span<ModelNode> ModelAsset::GetNodes() noexcept
	{
		return nodes;
	}

	inline std::span<const ModelNode> ModelAsset::GetNodes() const noexcept
	{
		return nodes;
	}
}
