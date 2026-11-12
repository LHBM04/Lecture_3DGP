#pragma once

#include <dxgi.h>

#include "../Core/Common.hpp"

#include "AssetImporter.hpp"
#include "MaterialAsset.hpp"

namespace TUK::Framework
{
	class AssetSubsystem;
	class GPUDevice;

	class MaterialAssetImporter final : public AssetImporter<MaterialAsset>
	{
	public:
		explicit MaterialAssetImporter(
			AssetSubsystem* owner,
			const GPUDevice& device) noexcept;

		[[nodiscard]] std::expected<std::unique_ptr<MaterialAsset>, std::string> Import(
			const std::filesystem::path& path) const override;

	private:
		const GPUDevice& device;
		DXGI_FORMAT targetFormat;
	};

	inline MaterialAssetImporter::MaterialAssetImporter(
		AssetSubsystem* owner,
		const GPUDevice& device) noexcept
		: AssetImporter<MaterialAsset>(owner)
		, device(device)
		, targetFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
	{
	}
}
