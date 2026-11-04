#pragma once

#include <expected>
#include <filesystem>
#include <memory>
#include <span>
#include <string>

#include <directxtk12/VertexTypes.h>

#include "../Core/Common.hpp"

#include "../Graphics/GPUMesh.hpp"

#include "AssetLoader.hpp"
#include "ModelAsset.hpp"

namespace TUK::Framework
{
	class AssetSubsystem;
	class GPUDevice;

	class ModelAssetLoader final : public AssetLoader
	{
	public:
		using AssetType = ModelAsset;

		ModelAssetLoader(const GPUDevice& device, AssetSubsystem& assets) noexcept;
		~ModelAssetLoader() noexcept override = default;

		[[nodiscard]] std::expected<std::unique_ptr<Asset>, std::string> Load(
			const std::filesystem::path& path) const override;

	private:
		const GPUDevice& device;
		AssetSubsystem& assets;
	};
}
