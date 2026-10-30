#pragma once

#include "AssetLoader.hpp"
#include "ShaderAsset.hpp"

namespace TUK::Framework
{
	class D3DDevice;

	class ShaderAssetLoader final : public AssetLoader
	{
	public:
		using AssetType = ShaderAsset;

		explicit ShaderAssetLoader(const D3DDevice& device) noexcept;
		~ShaderAssetLoader() noexcept override = default;

		[[nodiscard]] std::expected<std::unique_ptr<Asset>, std::string> Load(
			const std::filesystem::path& path) const override;

	private:
		const D3DDevice& device;
	};
}
