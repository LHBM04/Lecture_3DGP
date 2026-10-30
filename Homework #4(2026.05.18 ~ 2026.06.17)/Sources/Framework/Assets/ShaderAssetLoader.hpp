#pragma once

#include "AssetLoader.hpp"

namespace TUK::Framework
{
	class GPUDevice;
	class ShaderAsset;

	class ShaderAssetLoader final : public AssetLoader
	{
	public:
		using AssetType = ShaderAsset;

		explicit ShaderAssetLoader(const GPUDevice& device) noexcept;
		~ShaderAssetLoader() noexcept override = default;

		[[nodiscard]] std::expected<std::unique_ptr<Asset>, std::string> Load(
			const std::filesystem::path& path) const override;

	private:
		const GPUDevice& device;
	};
}
