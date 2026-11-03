#pragma once

#include "../Core/Common.hpp"

#include "AssetLoader.hpp"
#include "ShaderAsset.hpp"

namespace TUK::Framework
{
	class GPUDevice;

	class ShaderAssetLoader final : public AssetLoader
	{
	public:
		using AssetType = ShaderAsset;

		ShaderAssetLoader() noexcept = default;
		explicit ShaderAssetLoader([[maybe_unused]] const GPUDevice& device) noexcept;
		~ShaderAssetLoader() noexcept override = default;

		[[nodiscard]] std::expected<std::unique_ptr<Asset>, std::string> Load(
			const std::filesystem::path& path) const override;
	};

	inline ShaderAssetLoader::ShaderAssetLoader([[maybe_unused]] const GPUDevice& device) noexcept
	{
	}
}
