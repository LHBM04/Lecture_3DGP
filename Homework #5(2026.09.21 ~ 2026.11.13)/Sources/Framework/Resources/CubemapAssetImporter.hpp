#pragma once

#include "AssetImporter.hpp"
#include "CubemapAsset.hpp"

namespace TUK::Framework
{
	class AssetSubsystem;
	class GPUDevice;

	class CubemapAssetImporter final : public AssetImporter<CubemapAsset>
	{
	public:
		explicit CubemapAssetImporter(AssetSubsystem* owner, const GPUDevice& device) noexcept;

		[[nodiscard]] std::expected<std::unique_ptr<CubemapAsset>, std::string> Import(
			const std::filesystem::path& path) const override;

	private:
		const GPUDevice& device;
	};

	inline CubemapAssetImporter::CubemapAssetImporter(AssetSubsystem* owner, const GPUDevice& device) noexcept
		: AssetImporter<CubemapAsset>(owner)
		, device(device)
	{
	}
}
