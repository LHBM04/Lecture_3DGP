#pragma once

#include "AssetLoader.hpp"
#include "ModelAsset.hpp"

namespace TUK::Framework
{
	class D3DDevice;

	class ModelAssetLoader final : public AssetLoader
	{
	public:
		using AssetType = ModelAsset;

		explicit ModelAssetLoader(const D3DDevice& device) noexcept;
		~ModelAssetLoader() noexcept override = default;

		[[nodiscard]] std::expected<std::unique_ptr<Asset>, std::string> Load(
			const std::filesystem::path& path) const override;

	private:
		const D3DDevice& device;
	};
}
