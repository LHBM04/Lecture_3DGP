#pragma once

#include "../Core/Common.hpp"

#include "AssetImporter.hpp"
#include "MeshAsset.hpp"

namespace TUK::Framework
{
	class AssetSubsystem;
	class GPUDevice;

	class MeshAssetImporter final : public AssetImporter<MeshAsset>
	{
	public:
		explicit MeshAssetImporter(AssetSubsystem* owner, const GPUDevice& device) noexcept;

		[[nodiscard]] std::expected<std::unique_ptr<MeshAsset>, std::string> Import(
			const std::filesystem::path& path) const override;

	private:
		const GPUDevice& device;
	};

	inline MeshAssetImporter::MeshAssetImporter(AssetSubsystem* owner, const GPUDevice& device) noexcept
		: AssetImporter<MeshAsset>(owner)
		, device(device)
	{
	}
}
