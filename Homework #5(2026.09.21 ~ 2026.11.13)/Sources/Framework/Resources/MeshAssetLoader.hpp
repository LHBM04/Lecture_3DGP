#pragma once

#include <expected>
#include <filesystem>
#include <memory>
#include <string>

#include "../Core/Common.hpp"

#include "AssetLoader.hpp"
#include "MeshAsset.hpp"

namespace TUK::Framework
{
	class GPUDevice;

	class MeshAssetLoader final : public AssetLoader
	{
	public:
		using AssetType = MeshAsset;

		explicit MeshAssetLoader(const GPUDevice& device) noexcept;

		[[nodiscard]] std::expected<std::unique_ptr<Asset>, std::string> Load(
			const std::filesystem::path& path) const override;

	private:
		const GPUDevice& device;
	};
}
