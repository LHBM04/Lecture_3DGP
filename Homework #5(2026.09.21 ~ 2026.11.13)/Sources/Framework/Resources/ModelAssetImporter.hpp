#pragma once

#include "../Core/Common.hpp"

#include "AssetImporter.hpp"
#include "ModelAsset.hpp"

namespace TUK::Framework
{
	class ModelAssetImporter final : public AssetImporter<ModelAsset>
	{
	public:
		using AssetImporter<ModelAsset>::AssetImporter;

		[[nodiscard]] std::expected<std::unique_ptr<ModelAsset>, std::string> Import(
			const std::filesystem::path& path) const override;
	};
}
