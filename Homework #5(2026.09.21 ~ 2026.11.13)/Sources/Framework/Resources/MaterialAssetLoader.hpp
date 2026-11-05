#pragma once

#include "AssetLoader.hpp"
#include "MaterialAsset.hpp"

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class MaterialAssetLoader final : public AssetLoader
	{
	public:
		using AssetType = MaterialAsset;

		MaterialAssetLoader() noexcept = default;
		~MaterialAssetLoader() noexcept override = default;

		[[nodiscard]] std::expected<std::unique_ptr<Asset>, std::string> Load(
			const std::filesystem::path& path) const override;

	};
}
