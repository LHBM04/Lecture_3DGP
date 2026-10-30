#pragma once

#include <utility>

#include "Asset.hpp"
#include "Framework/Graphics/GPUMesh.hpp"

namespace TUK::Framework
{
	struct ModelVertex final
	{
		float position[3]{};
		float texcoord[2]{};
		float normal[3]{};
	};

	class ModelAsset : public Asset
	{
	public:
		explicit ModelAsset(GPUMesh mesh) noexcept;
		~ModelAsset() noexcept override = default;

		ModelAsset(const ModelAsset&) = delete;
		ModelAsset& operator=(const ModelAsset&) = delete;

		ModelAsset(ModelAsset&&) noexcept = default;
		ModelAsset& operator=(ModelAsset&&) noexcept = default;

		[[nodiscard]] const GPUMesh& GetMesh() const noexcept;

	private:
		GPUMesh mesh;
	};

	inline ModelAsset::ModelAsset(GPUMesh mesh) noexcept
		: mesh(std::move(mesh))
	{
	}

	inline const GPUMesh& ModelAsset::GetMesh() const noexcept
	{
		return mesh;
	}
}
