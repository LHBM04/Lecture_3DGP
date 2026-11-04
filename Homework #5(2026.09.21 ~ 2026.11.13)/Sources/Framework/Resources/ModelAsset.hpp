#pragma once

#include <filesystem>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <directxtk12/VertexTypes.h>

#include "Framework/Graphics/GPUMesh.hpp"

#include "Framework/Math/Matrix4x4.hpp"
#include "Framework/Math/Quaternion.hpp"
#include "Framework/Math/Vector3D.hpp"

#include "Asset.hpp"
#include "MaterialAsset.hpp"

#include "../Core/Common.hpp"

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

	struct ModelReference final
	{
		std::string meshName;
		std::string materialName;
		Matrix4x4<f32> localToModel;
	};

	struct ModelPart final
	{
		GPUMesh mesh;
		const MaterialAsset* material;
		Matrix4x4<f32> localToModel;
	};

	class ModelAsset : public Asset
	{
	public:
		ModelAsset() noexcept = default;
		[[nodiscard]] std::string Serialize() const override;
		[[nodiscard]] bool Deserialize(std::string_view text) override;
		[[nodiscard]] bool Upload(const GPUDevice& device, AssetSubsystem& assets, const std::filesystem::path& path);

		~ModelAsset() noexcept override = default;

		ModelAsset(const ModelAsset&) = delete;
		ModelAsset& operator=(const ModelAsset&) = delete;
		ModelAsset(ModelAsset&&) noexcept = default;
		ModelAsset& operator=(ModelAsset&&) noexcept = default;

		[[nodiscard]] std::string_view GetName() const noexcept;
		[[nodiscard]] std::span<const ModelNode> GetNodes() const noexcept;
		[[nodiscard]] std::span<const ModelReference> GetReferences() const noexcept;
		[[nodiscard]] std::span<const ModelPart> GetParts() const noexcept;

	private:
		std::string name;
		std::vector<ModelNode> nodes;
		std::vector<ModelReference> references;
		std::vector<ModelPart> parts;
	};

	inline std::string_view ModelAsset::GetName() const noexcept
	{
		return name;
	}

	inline std::span<const ModelNode> ModelAsset::GetNodes() const noexcept
	{
		return nodes;
	}

	inline std::span<const ModelReference> ModelAsset::GetReferences() const noexcept
	{
		return references;
	}

	inline std::span<const ModelPart> ModelAsset::GetParts() const noexcept
	{
		return parts;
	}
}
