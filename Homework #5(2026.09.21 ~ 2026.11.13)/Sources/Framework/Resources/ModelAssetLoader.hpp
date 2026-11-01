#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <memory>
#include <span>
#include <string>

#include <directxtk12/VertexTypes.h>

#include "AssetLoader.hpp"
#include "ModelAsset.hpp"

#include "Framework/Graphics/GPUMesh.hpp"

#include "../Core/Common.hpp"

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
		[[nodiscard]] std::expected<GPUBuffer, std::string> CreateUploadBuffer(std::span<const std::byte> bytes) const;
		[[nodiscard]] std::expected<GPUMesh, std::string> CreateMesh(
			std::span<const DirectX::VertexPositionNormalColor> vertices,
			std::span<const u32> indices) const;

		const GPUDevice& device;
		AssetSubsystem& assets;
	};
}
