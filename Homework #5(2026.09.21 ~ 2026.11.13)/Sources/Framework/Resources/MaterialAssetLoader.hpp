#pragma once

#include <dxgi.h>

#include "../Core/Common.hpp"

#include "AssetLoader.hpp"
#include "MaterialAsset.hpp"

namespace TUK::Framework
{
	class AssetSubsystem;
	class GPUDevice;

	class MaterialAssetLoader final : public AssetLoader<MaterialAsset>
	{
	public:
		explicit MaterialAssetLoader(
			AssetSubsystem* owner,
			const GPUDevice& device) noexcept;

		[[nodiscard]] std::expected<std::unique_ptr<MaterialAsset>, std::string> Load(
			const std::filesystem::path& path) const override;

	private:
		const GPUDevice& device;
		DXGI_FORMAT targetFormat;
	};

	inline MaterialAssetLoader::MaterialAssetLoader(
		AssetSubsystem* owner,
		const GPUDevice& device) noexcept
		: AssetLoader<MaterialAsset>(owner)
		, device(device)
		, targetFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
	{
	}
}
