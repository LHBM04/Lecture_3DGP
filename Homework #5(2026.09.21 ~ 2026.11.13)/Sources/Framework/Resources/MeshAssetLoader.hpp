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
	class AssetSubsystem;
	class GPUDevice;

	class MeshAssetLoader final : public AssetLoader<MeshAsset>
	{
	public:
		explicit MeshAssetLoader(AssetSubsystem* owner, const GPUDevice& device) noexcept;

		[[nodiscard]] std::expected<std::unique_ptr<MeshAsset>, std::string> Load(
			const std::filesystem::path& path) const override;

	private:
		const GPUDevice& device;
	};

	inline MeshAssetLoader::MeshAssetLoader(AssetSubsystem* owner, const GPUDevice& device) noexcept
		: AssetLoader<MeshAsset>(owner)
		, device(device)
	{
	}
}
