#pragma once

#include "../Core/Common.hpp"

#include "AssetImporter.hpp"
#include "TerrainAsset.hpp"

namespace TUK::Framework
{
	class AssetSubsystem;
	class GPUDevice;

	class TerrainAssetImporter final : public AssetImporter<TerrainAsset>
	{
	public:
		explicit TerrainAssetImporter(AssetSubsystem* owner, const GPUDevice& device) noexcept;

		[[nodiscard]] std::expected<std::unique_ptr<TerrainAsset>, std::string> Import(
			const std::filesystem::path& path) const override;

	private:
		const GPUDevice& device;
	};

	inline TerrainAssetImporter::TerrainAssetImporter(AssetSubsystem* owner, const GPUDevice& device) noexcept
		: AssetImporter<TerrainAsset>(owner)
		, device(device)
	{
	}
}
